#include <map>

#include "Console.h"
#include "Core.h"
#include "DataDefs.h"
#include "Export.h"
#include "PluginManager.h"
#include "VersionInfo.h"

#include "python/Python.h"

#include "lua.h"
#include "lauxlib.h"
#include "LuaTools.h"
#include "LuaWrapper.h"

// This plugin uses size_t for pointers, since Python offers an easy way to
// convert PyLong objects to size_t's
static_assert(sizeof(void*) == sizeof(size_t), "void* and size_t are not the same size");

using namespace std;
using namespace DFHack;
using namespace DFHack::LuaWrapper;

DFHACK_PLUGIN("python");

#ifdef LINUX_BUILD
    #define PATHSEP ":"
#else
    #define PATHSEP ";"
#endif

color_ostream_proxy *py_console = nullptr;
set<compound_identity*> all_identities;

bool is_struct_identity(compound_identity *id)
{
    return all_identities.count(id) && dynamic_cast<struct_identity*>(id);
}

namespace DFObject {
    compound_identity *get_identity(PyObject *obj)
    {
        if (!PyObject_HasAttrString(obj, "_id"))
            return nullptr;

        PyObject *id_field = PyObject_GetAttrString(obj, "_id");
        compound_identity *id = nullptr;
        if (PyLong_Check(id_field))
        {
            id = (compound_identity*)PyLong_AsSize_t(id_field);
            if (!all_identities.count(id))
                id = nullptr;
        }
        Py_DECREF(id_field);
        return id;
    }

    void *get_address(PyObject *obj)
    {
        if (!PyObject_HasAttrString(obj, "_address"))
            return nullptr;

        PyObject *address_field = PyObject_GetAttrString(obj, "_address");
        void *addr = nullptr;
        if (PyLong_Check(address_field))
        {
            addr = (void*)PyLong_AsSize_t(address_field);
            if (PyErr_Occurred())
            {
                addr = nullptr;
                PyErr_Print();
            }
        }
        Py_DECREF(address_field);
        return addr;
    }
}

namespace pylua
{
    PyObject *class_DFObject = nullptr;
    PyObject *func_reinterpret_cast = nullptr;

    bool push(lua_State *L, PyObject *obj)
    {
        if (obj == Py_None)
            lua_pushnil(L);
        else if (PyBool_Check(obj))
            lua_pushboolean(L, obj == Py_True);
        else if (PyLong_Check(obj))
            lua_pushinteger(L, PyLong_AsLongLong(obj));
        else if (PyFloat_Check(obj))
            lua_pushnumber(L, PyFloat_AsDouble(obj));
        else if (PyUnicode_Check(obj))
            lua_pushstring(L, PyUnicode_AsUTF8(obj));
        else
        {
            if (class_DFObject && PyObject_IsInstance(obj, class_DFObject))
            {
                compound_identity *id = DFObject::get_identity(obj);
                void *addr = DFObject::get_address(obj);
                Lua::PushDFObject(L, id, addr);
            }
            else
                return false;
        }
        return true;
    }

    type_identity *get_object_identity(lua_State *L, int idx, const char *ctx = "get_object_identity")
    {
        if (!lua_isuserdata(L, idx))
            luaL_error(L, "Object expected in %s", ctx);

        if (!lua_getmetatable(L, idx))
            luaL_error(L, "Invalid object in %s", ctx);

        // Extract identity from metatable
        lua_rawgetp(L, -1, &DFHACK_IDENTITY_FIELD_TOKEN);

        type_identity *id = (type_identity*)lua_touserdata(L, -1);

        if (!id)
            luaL_error(L, "Invalid object identity in %s", ctx);

        lua_pop(L, 2);

        return id;
    }

    void *get_object_address(lua_State *L, int idx)
    {
        auto ref = (DFRefHeader*)lua_touserdata(L, idx);
        return ref ? ref->ptr : nullptr;
    }

    int get_object_info(lua_State *L)
    {
        lua_pushlightuserdata(L, get_object_identity(L, 1, "get_object_info"));
        lua_pushlightuserdata(L, get_object_address(L, 1));
        return 2;
    }

    PyObject *topyobject(type_identity *id, void *addr)
    {
        if (all_identities.count((compound_identity*)id))
        {
            PyObject *args = PyTuple_New(2);
            PyTuple_SetItem(args, 0, PyLong_FromSize_t(size_t(id)));
            PyTuple_SetItem(args, 1, PyLong_FromSize_t(size_t(addr)));
            PyObject *ret = PyObject_CallObject(func_reinterpret_cast, args);
            Py_DECREF(args);
            return ret;
        }
        else
        {
            return PyErr_Format(PyExc_TypeError, "cannot convert %s %p to Python",
                "bad type", id);
        }
    }

    PyObject *topyobject(lua_State *L, int idx)
    {
        if (lua_isnone(L, idx))
            return PyErr_Format(PyExc_IndexError, "invalid lua index: %i", idx);
        else if (lua_isnil(L, idx))
            Py_RETURN_NONE;
        else if (lua_isboolean(L, idx))
            return PyBool_FromLong(long(lua_toboolean(L, idx)));
        else if (lua_isinteger(L, idx))
            return PyLong_FromLongLong(lua_tointeger(L, idx));
        else if (lua_isnumber(L, idx))
            return PyFloat_FromDouble(lua_tonumber(L, idx));
        else if (lua_isstring(L, idx))
            return PyUnicode_FromString(lua_tostring(L, idx));
        else if (Lua::IsDFObject(L, idx) == Lua::OBJ_REF && func_reinterpret_cast)
        {
            lua_dup(L);
            lua_pushcfunction(L, get_object_info);
            lua_swap(L);
            if (lua_pcall(L, 1, 2, 0) == LUA_OK)
            {
                auto id = (type_identity*)lua_touserdata(L, -2);
                void *addr = lua_touserdata(L, -1);
                return topyobject(id, addr);
            }
            else
            {
                return PyErr_Format(PyExc_RuntimeError, "Lua error: %s",
                    lua_isstring(L, -1) ? lua_tostring(L, -1) : "unknown");
            }

        }
        else
            return PyErr_Format(PyExc_TypeError, "cannot convert %s to Python",
                lua_typename(L, lua_type(L, idx)));
    }

    bool call_get_path(PyObject *path, vector<const char*> &out)
    {
        if (!PyTuple_Check(path))
        {
            PyErr_SetString(PyExc_TypeError, "path not a tuple");
            return false;
        }
        if (PyTuple_Size(path) <= 0)
        {
            PyErr_SetString(PyExc_ValueError, "path too small");
            return false;
        }
        out.clear();
        for (ssize_t i = 0; i < PyTuple_Size(path); i++)
        {
            PyObject *path_item = PyTuple_GetItem(path, i);
            if (PyUnicode_Check(path_item))
            {
                out.push_back(PyUnicode_AsUTF8(path_item));
            }
            else
            {
                PyErr_Format(PyExc_TypeError, "path item %i not unicode", int(i));
                return false;
            }
        }
        return true;
    }

    void recursive_get(lua_State *L)
    {
        int nargs = lua_gettop(L);
        lua_getglobal(L, luaL_checkstring(L, 1));
        for (int i = 2; i <= nargs; i++)
        {
            lua_getfield(L, -1, luaL_checkstring(L, i));
            lua_remove(L, -2);
        }
    }

    int can_call_helper(lua_State *L)
    {
        recursive_get(L);
        luaL_checktype(L, -1, LUA_TFUNCTION);
        return 1;
    }

    bool call_check(lua_State *L, const vector<const char*> &path)
    {
        lua_pushcfunction(L, can_call_helper);
        for (auto s : path)
            lua_pushstring(L, s);
        return (lua_pcall(L, path.size(), 1, 0) == LUA_OK);
    }

    int list_funcs_helper(lua_State *L)
    {
        recursive_get(L);
        int tbl = lua_gettop(L);
        luaL_checktype(L, tbl, LUA_TTABLE);
        lua_newtable(L);
        int func_list = lua_gettop(L);
        int func_i = 1;

        // first key
        lua_pushnil(L);
        while (lua_next(L, tbl))
        {
            if (lua_type(L, -1) == LUA_TFUNCTION && lua_type(L, -2) == LUA_TSTRING)
            {
                lua_pushinteger(L, func_i++); // push index
                lua_pushvalue(L, -3);       // copy name
                lua_settable(L, func_list); // func_list[func_i] = name
            }
            lua_pop(L, 1);  // pop value
        }

        return 1;
    }
}

class PyGILLock {
public:
    PyGILLock() : gstate(PyGILState_Ensure()) {}
    ~PyGILLock() { PyGILState_Release(gstate); }
private:
    PyGILState_STATE gstate;
};

namespace api {
    map<const char*, PyObject*(*)(PyObject*, PyObject*)> functions;
    #define API_FUNC_X(name, self, args) PyObject *name(PyObject*, PyObject*); \
        DFHACK_STATIC_ADD_TO_MAP(&functions, #name, name); \
        PyObject *name(PyObject *self, PyObject *args)
    #define API_FUNC(name) API_FUNC_X(name, self, args)

    API_FUNC(print)
    {
        const char *msg;
        int color = -100;
        if (!PyArg_ParseTuple(args, "z|i", &msg, &color))
            return nullptr;

        if (color != -100)
            py_console->color(color_value(color));
        py_console->print("%s", msg);
        if (color != -100)
            py_console->color(COLOR_RESET);

        Py_RETURN_NONE;
    }

    API_FUNC(printerr)
    {
        const char *msg;
        if (!PyArg_ParseTuple(args, "z", &msg))
            return nullptr;

        py_console->printerr("%s", msg);

        Py_RETURN_NONE;
    }

    API_FUNC(get_global_address)
    {
        const char *name;
        if (!PyArg_ParseTuple(args, "z", &name))
            return nullptr;

        return PyLong_FromSize_t(size_t(Core::getInstance().vinfo->getAddress(name)));
    }

    API_FUNC(all_type_ids)
    {
        if (all_identities.empty())
        {
            auto ids = compound_identity::getTopScope();
            for (compound_identity *id : ids)
                all_identities.insert(id);
        }
        PyObject *dict = PyDict_New();
        for (auto id : all_identities)
        {
            PyDict_SetItem(dict,
                PyLong_FromSize_t(size_t(id)),
                PyUnicode_FromString(id->getName()));
        }
        return dict;
    }

    API_FUNC(type_name)
    {
        compound_identity *id;
        if (!PyArg_ParseTuple(args, "n", (ssize_t*)&id))
            return nullptr;
        if (all_identities.count(id))
        {
            return PyUnicode_FromString(id->getName());
        }
        else
        {
            return PyErr_Format(PyExc_TypeError, "unknown DF type: %p", id);
        }
    }

    API_FUNC(type_is_subclass)
    {
        struct_identity *id_parent, *id_subclass;
        if (!PyArg_ParseTuple(args, "nn", (ssize_t*)&id_parent, (ssize_t*)&id_subclass))
            return nullptr;

        if (id_parent == id_subclass)
            Py_RETURN_TRUE;

        if (!is_struct_identity(id_parent))
        {
            if (all_identities.count(id_parent))
                Py_RETURN_FALSE;
            return PyErr_Format(PyExc_TypeError, "unknown parent ID: %p", id_parent);
        }
        if (!is_struct_identity(id_subclass))
        {
            if (all_identities.count(id_subclass))
                Py_RETURN_FALSE;
            return PyErr_Format(PyExc_TypeError, "unknown subclass ID: %p", id_subclass);
        }

        while (id_subclass)
        {
            if (id_subclass == id_parent)
                Py_RETURN_TRUE;
            id_subclass = id_subclass->getParent();
        }
        Py_RETURN_FALSE;
    }

    API_FUNC(lua_can_call)
    {
        if (PyTuple_Size(args) != 1)
            return PyErr_Format(PyExc_TypeError, "%s expected 1 argument, got %i",
                __FUNCTION__, int(PyTuple_Size(args)));
        vector<const char*> path;
        if (!pylua::call_get_path(PyTuple_GetItem(args, 0), path))
            return nullptr;

        lua_State *L = Lua::Core::State;
        bool can_call = pylua::call_check(L, path);
        lua_settop(L, 0);
        return PyBool_FromLong(can_call);
    }

    API_FUNC(lua_list_funcs)
    {
        lua_State *L = Lua::Core::State;
        Lua::StackUnwinder top(L);

        vector<const char*> path;
        if (!pylua::call_get_path(args, path))
            return nullptr;

        lua_pushcfunction(L, pylua::list_funcs_helper);
        for (auto s : path)
            lua_pushstring(L, s);

        if (lua_pcall(L, path.size(), 1, 0) != LUA_OK)
            return PyErr_Format(PyExc_RuntimeError, "Lua error: %s",
                lua_isstring(L, -1) ? lua_tostring(L, -1) : "unknown");

        if (!lua_istable(L, -1))
            return PyErr_Format(PyExc_TypeError,
                "list_funcs_helper did not return a table");

        vector<const char*> names;
        lua_pushnil(L);
        while (lua_next(L, -2))
        {
            names.push_back(lua_tostring(L, -1));
            lua_pop(L, 1);
        }

        PyObject *list = PyList_New(names.size());
        for (size_t i = 0; i < names.size(); i++)
            PyList_SetItem(list, i, PyUnicode_FromString(names[i]));
        return list;
    }

    API_FUNC(lua_call_func)
    {
        if (PyTuple_Size(args) < 1)
            return PyErr_Format(PyExc_TypeError, "%s expected at least 1 argument, got %i",
                __FUNCTION__, int(PyTuple_Size(args)));
        vector<const char*> path;
        if (!pylua::call_get_path(PyTuple_GetItem(args, 0), path))
            return nullptr;

        lua_State *L = Lua::Core::State;
        Lua::StackUnwinder unwind(L);

        bool can_call = pylua::call_check(L, path);
        if (!can_call)
            return PyErr_Format(PyExc_TypeError, "can't call function %s",
                path[path.size() - 1]);

        for (ssize_t i = 1; i < PyTuple_Size(args); i++)
        {
            if (!pylua::push(L, PyTuple_GetItem(args, i)))
                return PyErr_Format(PyExc_TypeError, "cannot pass arg %i (%R) to Lua",
                    int(i), PyTuple_GetItem(args, i));
        }

        if (!Lua::SafeCall(*py_console, L, PyTuple_Size(args) - 1, LUA_MULTRET, true))
            return PyErr_Format(PyExc_RuntimeError, "Lua error: %s",
                lua_isstring(L, -1) ? lua_tostring(L, -1) : "unknown");

        int nres = lua_gettop(L) - int(unwind);

        if (nres <= 0)
            Py_RETURN_NONE;
        else if (nres == 1)
            return pylua::topyobject(L, -1);
        else
        {
            PyObject *ret = PyTuple_New(nres);
            for (int i = 1; i <= nres; i++)
            {
                PyObject *retobj = pylua::topyobject(L, i);
                if (!retobj)
                    return nullptr;
                PyTuple_SetItem(ret, i - 1, retobj);
            }
            return ret;
        }
    }
}

PyMethodDef *dfhack_methods = nullptr;

PyModuleDef dfhack_module = {
    PyModuleDef_HEAD_INIT, "_dfhack", nullptr, -1, dfhack_methods,
    0, 0, 0, 0
};

PyObject* dfhack_init()
{
    if (!dfhack_methods)
    {
        dfhack_methods = new PyMethodDef[api::functions.size() + 1];
        size_t i = 0;
        for (auto it : api::functions)
        {
            dfhack_methods[i++] = {it.first, it.second, METH_VARARGS, 0};
        }
        dfhack_methods[i] = {0, 0, 0, 0};
    }
    dfhack_module.m_methods = dfhack_methods;
    return PyModule_Create(&dfhack_module);
}

PyObject *import_module(color_ostream &out, const char *module)
{
    PyObject *mod = PyImport_ImportModule(module);
    if (!mod)
    {
        out.printerr("import failed: %s\n", module);
        PyErr_Clear();
    }
    return mod;
}

PyObject *import_from(color_ostream &out, PyObject *module, const char *name)
{
    PyObject *ret = PyObject_GetAttrString(module, name);
    if (!ret)
    {
        out.printerr("import failed: %s\n", name);
        PyErr_Clear();
    }
    return ret;
}

bool py_startup(color_ostream &out) {
    PyImport_AppendInittab("_dfhack", dfhack_init);

    Py_SetProgramName(Py_DecodeLocale("dfhack", nullptr));

    Py_SetPath(Py_DecodeLocale(
        "./hack/python" PATHSEP
        "./hack/python/stdlib-dynload" PATHSEP
        "./hack/python/stdlib-python.zip",
        nullptr));

    Py_InitializeEx(0);
    PyEval_InitThreads();
    // PyEval_ReleaseThread(PyThreadState_Get());
    // PyEval_ReleaseLock();

    PyObject *df_module, *dfhack_module;
    if (!(df_module = import_module(out, "df")) || !(dfhack_module = import_module(out, "dfhack")))
        return false;

    pylua::class_DFObject = import_from(out, df_module, "DFObject");
    pylua::func_reinterpret_cast = import_from(out, df_module, "reinterpret_cast");

    Py_DECREF(df_module);
    Py_DECREF(dfhack_module);

    PyRun_SimpleString("import df, dfhack");

    return true;
}

bool py_cleanup(color_ostream &out)
{
    Py_CLEAR(pylua::class_DFObject);
    Py_CLEAR(pylua::func_reinterpret_cast);

    return true;
}

command_result run_script(string name, vector<string> args)
{
    std::string path = Core::getInstance().findScript(name + ".py");
    if (path.empty())
        return CR_NOT_FOUND;

    PyObject *dfhack = PyImport_ImportModule("dfhack");
    if (!dfhack)
    {
        Core::printerr("could not import dfhack module\n");
        if (PyErr_Occurred())
            PyErr_Print();
        return CR_FAILURE;
    }
    PyObject *run_script = PyObject_GetAttrString(dfhack, "run_script");
    if (run_script)
    {
        PyObject *pargs = PyTuple_New(ssize_t(args.size()) + 1);
        PyTuple_SetItem(pargs, 0, PyUnicode_FromString(path.c_str()));
        for (size_t i = 0; i < args.size(); i++)
        {
            PyObject *arg = PyUnicode_FromString(args[i].c_str());
            if (arg)
            {
                // steals reference to arg
                PyTuple_SetItem(pargs, ssize_t(i) + 1, arg);
            }
        }
        PyObject *ret = PyObject_CallObject(run_script, pargs);
        Py_DECREF(pargs);
        Py_XDECREF(ret);
    }

    if (PyErr_Occurred())
        PyErr_Print();

    Py_XDECREF(run_script);
    Py_DECREF(dfhack);

    return CR_OK;
}

command_result df_py (color_ostream &out, std::vector <std::string> & parameters);
command_result df_pyscript (color_ostream &out, std::vector <std::string> & parameters);

DFhackCExport command_result plugin_init (color_ostream &out, std::vector <PluginCommand> &commands)
{
    py_console = new color_ostream_proxy(Core::getInstance().getConsole());

    if (!py_startup(out))
        return CR_FAILURE;

    commands.push_back(PluginCommand(
        "py", "", df_py, false
    ));
    commands.push_back(PluginCommand(
        "pyscript", "", df_pyscript, false
    ));
    return CR_OK;
}

DFhackCExport command_result plugin_shutdown (color_ostream &out)
{
    if (!py_cleanup(out))
        out.printerr("py_cleanup failed\n");

    // PyGILState_Ensure();
    if (Py_FinalizeEx())
        out.printerr("Py_FinalizeEx failed\n");

    // Remove _dfhack from PyImport_Inittab. Otherwise, a stale entry will be
    // left behind, which could crash when reloading this plugin.
    for (int i = 0; PyImport_Inittab[i].name; i++) {
        if (string(PyImport_Inittab[i].name) == "_dfhack") {
            PyImport_Inittab[i].name = nullptr;
            PyImport_Inittab[i].initfunc = nullptr;
            break;
        }
    }

    delete py_console;

    if (dfhack_methods)
    {
        delete[] dfhack_methods;
        dfhack_methods = nullptr;
    }

    return CR_OK;
}

command_result df_py (color_ostream &out, std::vector <std::string> & parameters)
{
    if (parameters.empty())
        return CR_WRONG_USAGE;

    CoreSuspender suspend;
    PyRun_SimpleString(parameters[0].c_str());
    return CR_OK;
}

command_result df_pyscript (color_ostream &out, vector<string> &args)
{
    if (args.empty())
        return CR_WRONG_USAGE;

    CoreSuspender suspend;
    string script = args[0];
    args.erase(args.begin());
    auto cr = run_script(script, args);
    if (cr == CR_NOT_FOUND)
        out.printerr("not found: %s\n", script.c_str());

    return cr;
}

