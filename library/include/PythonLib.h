#include "PluginManager.h"

namespace CPython {
    #include "python/Python.h"
}

namespace DFHack {
    namespace Python {
        DFHACK_EXPORT bool Init();
        DFHACK_EXPORT bool Shutdown();
        DFHACK_EXPORT bool isLoaded();
        DFHACK_EXPORT DFLibrary *GetHandle();
        DFHACK_EXPORT bool LoadLibrary(std::string path = "");

#define DFH_PY_DEFINE
#define DFH_PY_FUNC(rtype, name, args) DFHACK_EXPORT extern rtype (*name) args;
#include "PythonLibStubs.h"
#undef DFH_PY_FUNC
#undef DFH_PY_DEFINE
    }
}
