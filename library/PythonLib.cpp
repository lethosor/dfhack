#include "PythonLib.h"
#include <string>

using namespace DFHack;
static bool is_loaded = false;
static DFLibrary *py_handle = NULL;
bool Python::isLoaded() { return is_loaded; }
DFLibrary *Python::GetHandle() { return py_handle; }

#ifdef LINUX_BUILD
#include <dlfcn.h>
DFLibrary *load_library(const char *path)
{
    DFLibrary *ret = (DFLibrary*)dlopen(path, RTLD_NOW | RTLD_GLOBAL);
    if (!ret)
        std::cerr << dlerror() << std::endl;
    return ret;
}
#else
#define load_library DFHack::OpenPlugin
#endif

bool Python::Init()
{
#ifdef LINUX_BUILD
    #define delim ":"
#else
    #define delim ";"
#endif
    if (!LoadLibrary())
    {
        fprintf(stderr, "Failed to load python library\n");
        return false;
    }
    if (!Py_IsInitialized())
    {
        Py_SetProgramName((wchar_t*)L"dfhack");
        Py_SetPath(L"hack/python" delim
            "hack/python/stdlib" delim
            "hack/python/stdlib/lib-dynload");
        Py_InitializeEx(0); // Don't register signal handlers
    }
    return is_loaded;
#undef delim
}

bool Python::Shutdown()
{
    Py_Finalize();
    return true;
}

bool bind(DFLibrary *lib, const char *name, void **dest)
{
    *dest = LookupPlugin(lib, name);
    if (*dest == NULL)
        fprintf(stderr, "Python: NULL symbol: %s\n", name);
    return *dest != NULL;
}

#define DFH_PY_BIND(lib, sym) if (!bind(lib, #sym, (void**)&DFHack::Python::sym)) missing_symbols = true
bool Python::LoadLibrary(std::string path)
{
    is_loaded = false;
    if (path == "")
    {
#ifdef _DARWIN
        path = "hack/libpython3.4m.dylib";
#elif defined(_LINUX)
        path = "hack/libpython3.4m.so";
#else
        path = "./libpython3.4m.dll";
#endif
    }
    DFLibrary *py = load_library(path.c_str());
    bool missing_symbols = false;
    if (!py)
        return false;
#define DFH_PY_FUNC(rtype, name, args) DFH_PY_BIND(py, name)
#include "PythonLibStubs.h"
#undef DFH_PY_FUNC
    if (missing_symbols)
        return false;
    py_handle = py;
    is_loaded = true;
    return true;
}
#undef DFH_PY_BIND

namespace DFHack { namespace Python {
#define DFH_PY_FUNC(rtype, name, args) rtype (*name) args = 0;
#include "PythonLibStubs.h"
#undef DFH_PY_FUNC
}}

