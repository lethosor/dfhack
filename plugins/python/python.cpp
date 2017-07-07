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

// This plugin uses size_t for pointers, since Python offers an easy way to
// convert PyLong objects to size_t's
static_assert(sizeof(void*) == sizeof(size_t), "void* and size_t are not the same size");

using namespace std;
using namespace DFHack;

DFHACK_PLUGIN("python");

#ifdef LINUX_BUILD
    #define PATHSEP ":"
#else
    #define PATHSEP ";"
#endif

color_ostream_proxy *py_console = 0;
set<compound_identity*> all_identities;

namespace pylua
{
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
            return false;
        return true;
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

    int can_call_helper(lua_State *L)
    {
        int nargs = lua_gettop(L);
        lua_getglobal(L, luaL_checkstring(L, 1));
        for (int i = 2; i <= nargs; i++)
        {
            lua_getfield(L, -1, luaL_checkstring(L, i));
            lua_remove(L, -2);
        }
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

        if (lua_pcall(L, PyTuple_Size(args) - 1, LUA_MULTRET, 0) != LUA_OK)
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

    PyRun_SimpleString("import dfhack");

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
    if (!py_startup(out))
        return CR_FAILURE;

    py_console = new color_ostream_proxy(Core::getInstance().getConsole());

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

