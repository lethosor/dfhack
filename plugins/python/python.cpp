#include "Console.h"
#include "Core.h"
#include "DataDefs.h"
#include "Export.h"
#include "PluginManager.h"

#include "python/Python.h"

using namespace std;
using namespace DFHack;

DFHACK_PLUGIN("python");

#ifdef LINUX_BUILD
    #define PATHSEP ":"
#else
    #define PATHSEP ";"
#endif

color_ostream_proxy *py_console = 0;

PyObject *dfhack_print(PyObject *self, PyObject *args) {
    // Core::print("dfhack_print()\n");
    const char *msg;
    int color = -100;
    if (!PyArg_ParseTuple(args, "z|i", &msg, &color))
        return nullptr;

    if (color != -100)
        py_console->color(color_value(color));
    py_console->print(msg);
    if (color != -100)
        py_console->color(COLOR_RESET);

    Py_RETURN_NONE;
}

PyObject *dfhack_printerr(PyObject *self, PyObject *args) {
    const char *msg;
    if (!PyArg_ParseTuple(args, "z", &msg))
        return nullptr;

    py_console->printerr(msg);

    Py_RETURN_NONE;
}

PyObject *dfhack_call(PyObject *self, PyObject *args) {
    ssize_t len = PyTuple_Size(args);
    cout << "dfhack_call(): " << len << " arguments" << endl;
    for (ssize_t i = 0; i < len; i++) {
        PyObject *obj = PyTuple_GetItem(args, i);
        PyTypeObject *type = Py_TYPE(obj);
        cout << "  arg " << i << " = " << obj << ", type: " << type->tp_name << endl;
        if (PyBool_Check(obj)) {
            cout << "    as bool: "  << endl;
        }
        if (PyLong_Check(obj)) {
            cout << "    as long: " << PyLong_AsLong(obj) << endl;
        }
    }
    Py_RETURN_NONE;
}

PyMethodDef dfhack_methods[] = {
    {"print", dfhack_print, METH_VARARGS, 0},
    {"printerr", dfhack_printerr, METH_VARARGS, 0},
    {"call", dfhack_call, METH_VARARGS, 0},
    {0, 0, 0, 0}
};

PyModuleDef dfhack_module = {
    PyModuleDef_HEAD_INIT, "_dfhack", nullptr, -1, dfhack_methods,
    0, 0, 0, 0
};

PyObject* dfhack_init() {
    cout << "dfhack_init" << endl;
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
    Py_Initialize();

    out << PyImport_ImportModule("_dfhack") << endl;
    out << PyImport_ImportModule("sys") << endl;
    out << PyImport_ImportModule("math") << endl;
    out << PyImport_ImportModule("os.path") << endl;

    PyRun_SimpleString("import dfhack");

    return true;
}

command_result run_script(string name, vector<string> args)
{
    std::string path = Core::getInstance().findScript(name + ".py");
    if (path.empty())
        return CR_NOT_FOUND;

    PyObject *dfhack = PyImport_ImportModule("dfhack");
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
    Py_Finalize();
    delete py_console;

    return CR_OK;
}

command_result df_py (color_ostream &out, std::vector <std::string> & parameters)
{
    if (parameters.empty())
        return CR_WRONG_USAGE;
    PyRun_SimpleString(parameters[0].c_str());
    return CR_OK;
}

command_result df_pyscript (color_ostream &out, vector<string> &args)
{
    if (args.empty())
        return CR_WRONG_USAGE;

    string script = args[0];
    args.erase(args.begin());
    auto cr = run_script(script, args);
    if (cr == CR_NOT_FOUND)
        out.printerr("not found: %s\n", script.c_str());

    return cr;
}

