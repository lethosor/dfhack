// Generated - do not edit
// ['pythonrun.h', 'pyarena.h', 'pyport.h', 'pystate.h', 'object.h']
using CPython::PyCompilerFlags;
using CPython::PyOS_sighandler_t;
using CPython::PyArena;
using CPython::Py_uintptr_t;
using CPython::Py_intptr_t;
using CPython::Py_intptr_t;
using CPython::Py_intptr_t;
using CPython::Py_intptr_t;
using CPython::Py_ssize_t;
using CPython::Py_ssize_t;
using CPython::Py_hash_t;
using CPython::Py_uhash_t;
using CPython::Py_ssize_clean_t;
using CPython::Py_ssize_clean_t;
using CPython::PyInterpreterState;
using CPython::PyThreadState;
using CPython::PyGILState_STATE;
using CPython::PyInterpreterState;
using CPython::PyThreadState;
using CPython::Py_tracefunc;
using CPython::PyObject;
using CPython::_Py_Identifier;
using CPython::Py_buffer;
using CPython::PyTypeObject;
using CPython::PyHeapTypeObject;
using CPython::PyVarObject;
using CPython::PyNumberMethods;
using CPython::PySequenceMethods;
using CPython::PyMappingMethods;
using CPython::PyBufferProcs;
using CPython::PyType_Slot;
using CPython::PyType_Spec;
using CPython::PyTypeObject;
using CPython::inquiry;
using CPython::lenfunc;
using CPython::ssizeobjargproc;
using CPython::ssizessizeobjargproc;
using CPython::objobjargproc;
using CPython::getbufferproc;
using CPython::releasebufferproc;
using CPython::objobjproc;
using CPython::visitproc;
using CPython::traverseproc;
using CPython::freefunc;
using CPython::destructor;
using CPython::printfunc;
using CPython::setattrfunc;
using CPython::setattrofunc;
using CPython::hashfunc;
using CPython::descrsetfunc;
using CPython::initproc;
DFH_PY_FUNC(void, Py_SetProgramName, (wchar_t *));
DFH_PY_FUNC(wchar_t *, Py_GetProgramName, (void));
DFH_PY_FUNC(void, Py_SetPythonHome, (wchar_t *));
DFH_PY_FUNC(wchar_t *, Py_GetPythonHome, (void));
DFH_PY_FUNC(int, Py_SetStandardStreamEncoding, (const char *encoding,
                                             const char *errors));
DFH_PY_FUNC(void, Py_Initialize, (void));
DFH_PY_FUNC(void, Py_InitializeEx, (int));
DFH_PY_FUNC(void, _Py_InitializeEx_Private, (int, int));
DFH_PY_FUNC(void, Py_Finalize, (void));
DFH_PY_FUNC(int, Py_IsInitialized, (void));
DFH_PY_FUNC(PyThreadState *, Py_NewInterpreter, (void));
DFH_PY_FUNC(void, Py_EndInterpreter, (PyThreadState *));
DFH_PY_FUNC(int, PyRun_SimpleStringFlags, (const char *, PyCompilerFlags *));
DFH_PY_FUNC(int, PyRun_AnyFileFlags, (FILE *, const char *, PyCompilerFlags *));
DFH_PY_FUNC(int, PyRun_AnyFileExFlags, (
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    int closeit,
    PyCompilerFlags *flags));
DFH_PY_FUNC(int, PyRun_SimpleFileExFlags, (
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    int closeit,
    PyCompilerFlags *flags));
DFH_PY_FUNC(int, PyRun_InteractiveOneFlags, (
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    PyCompilerFlags *flags));
DFH_PY_FUNC(int, PyRun_InteractiveOneObject, (
    FILE *fp,
    PyObject *filename,
    PyCompilerFlags *flags));
DFH_PY_FUNC(int, PyRun_InteractiveLoopFlags, (
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    PyCompilerFlags *flags));
DFH_PY_FUNC(struct _mod *, PyParser_ASTFromString, (
    const char *s,
    const char *filename,       /* decoded from the filesystem encoding */
    int start,
    PyCompilerFlags *flags,
    PyArena *arena));
DFH_PY_FUNC(struct _mod *, PyParser_ASTFromStringObject, (
    const char *s,
    PyObject *filename,
    int start,
    PyCompilerFlags *flags,
    PyArena *arena));
DFH_PY_FUNC(struct _mod *, PyParser_ASTFromFile, (
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    const char* enc,
    int start,
    char *ps1,
    char *ps2,
    PyCompilerFlags *flags,
    int *errcode,
    PyArena *arena));
DFH_PY_FUNC(struct _mod *, PyParser_ASTFromFileObject, (
    FILE *fp,
    PyObject *filename,
    const char* enc,
    int start,
    char *ps1,
    char *ps2,
    PyCompilerFlags *flags,
    int *errcode,
    PyArena *arena));
DFH_PY_FUNC(struct _node *, PyParser_SimpleParseStringFlags, (const char *, int,
                                                           int));
DFH_PY_FUNC(struct _node *, PyParser_SimpleParseStringFlagsFilename, (const char *,
                                                                   const char *,
                                                                   int, int));
DFH_PY_FUNC(struct _node *, PyParser_SimpleParseFileFlags, (FILE *, const char *,
                                                         int, int));
DFH_PY_FUNC(PyObject *, PyRun_StringFlags, (const char *, int, PyObject *,
                                         PyObject *, PyCompilerFlags *));
DFH_PY_FUNC(PyObject *, PyRun_FileExFlags, (
    FILE *fp,
    const char *filename,       /* decoded from the filesystem encoding */
    int start,
    PyObject *globals,
    PyObject *locals,
    int closeit,
    PyCompilerFlags *flags));
DFH_PY_FUNC(PyObject *, Py_CompileString, (const char *, const char *, int));
DFH_PY_FUNC(PyObject *, Py_CompileStringExFlags, (
    const char *str,
    const char *filename,       /* decoded from the filesystem encoding */
    int start,
    PyCompilerFlags *flags,
    int optimize));
DFH_PY_FUNC(PyObject *, Py_CompileStringObject, (
    const char *str,
    PyObject *filename, int start,
    PyCompilerFlags *flags,
    int optimize));
DFH_PY_FUNC(struct symtable *, Py_SymtableString, (
    const char *str,
    const char *filename,       /* decoded from the filesystem encoding */
    int start));
DFH_PY_FUNC(struct symtable *, Py_SymtableStringObject, (
    const char *str,
    PyObject *filename,
    int start));
DFH_PY_FUNC(void, PyErr_Print, (void));
DFH_PY_FUNC(void, PyErr_PrintEx, (int));
DFH_PY_FUNC(void, PyErr_Display, (PyObject *, PyObject *, PyObject *));
DFH_PY_FUNC(void, Py_Exit, (int));
DFH_PY_FUNC(void, _Py_RestoreSignals, (void));
DFH_PY_FUNC(int, Py_FdIsInteractive, (FILE *, const char *));
DFH_PY_FUNC(int, Py_Main, (int argc, wchar_t **argv));
DFH_PY_FUNC(wchar_t *, Py_GetProgramFullPath, (void));
DFH_PY_FUNC(wchar_t *, Py_GetPrefix, (void));
DFH_PY_FUNC(wchar_t *, Py_GetExecPrefix, (void));
DFH_PY_FUNC(wchar_t *, Py_GetPath, (void));
DFH_PY_FUNC(void, Py_SetPath, (const wchar_t *));
DFH_PY_FUNC(const char *, Py_GetVersion, (void));
DFH_PY_FUNC(const char *, Py_GetPlatform, (void));
DFH_PY_FUNC(const char *, Py_GetCopyright, (void));
DFH_PY_FUNC(const char *, Py_GetCompiler, (void));
DFH_PY_FUNC(const char *, Py_GetBuildInfo, (void));
DFH_PY_FUNC(const char *, _Py_hgidentifier, (void));
DFH_PY_FUNC(const char *, _Py_hgversion, (void));
DFH_PY_FUNC(PyObject *, _PyBuiltin_Init, (void));
DFH_PY_FUNC(PyObject *, _PySys_Init, (void));
DFH_PY_FUNC(void, _PyImport_Init, (void));
DFH_PY_FUNC(void, _PyExc_Init, (PyObject * bltinmod));
DFH_PY_FUNC(void, _PyImportHooks_Init, (void));
DFH_PY_FUNC(int, _PyFrame_Init, (void));
DFH_PY_FUNC(int, _PyFloat_Init, (void));
DFH_PY_FUNC(int, PyByteArray_Init, (void));
DFH_PY_FUNC(void, _PyRandom_Init, (void));
DFH_PY_FUNC(void, _PyExc_Fini, (void));
DFH_PY_FUNC(void, _PyImport_Fini, (void));
DFH_PY_FUNC(void, PyMethod_Fini, (void));
DFH_PY_FUNC(void, PyFrame_Fini, (void));
DFH_PY_FUNC(void, PyCFunction_Fini, (void));
DFH_PY_FUNC(void, PyDict_Fini, (void));
DFH_PY_FUNC(void, PyTuple_Fini, (void));
DFH_PY_FUNC(void, PyList_Fini, (void));
DFH_PY_FUNC(void, PySet_Fini, (void));
DFH_PY_FUNC(void, PyBytes_Fini, (void));
DFH_PY_FUNC(void, PyByteArray_Fini, (void));
DFH_PY_FUNC(void, PyFloat_Fini, (void));
DFH_PY_FUNC(void, PyOS_FiniInterrupts, (void));
DFH_PY_FUNC(void, _PyGC_DumpShutdownStats, (void));
DFH_PY_FUNC(void, _PyGC_Fini, (void));
DFH_PY_FUNC(void, PySlice_Fini, (void));
DFH_PY_FUNC(void, _PyType_Fini, (void));
DFH_PY_FUNC(void, _PyRandom_Fini, (void));
DFH_PY_FUNC(char *, PyOS_Readline, (FILE *, FILE *, const char *));
DFH_PY_FUNC(int, PyOS_CheckStack, (void));
DFH_PY_FUNC(PyOS_sighandler_t, PyOS_getsig, (int));
DFH_PY_FUNC(PyOS_sighandler_t, PyOS_setsig, (int, PyOS_sighandler_t));
DFH_PY_FUNC(int, _PyOS_URandom , (void *buffer, Py_ssize_t size));
DFH_PY_FUNC(PyArena *, PyArena_New, (void));
DFH_PY_FUNC(void, PyArena_Free, (PyArena *));
DFH_PY_FUNC(void *, PyArena_Malloc, (PyArena *, size_t size));
DFH_PY_FUNC(int, PyArena_AddPyObject, (PyArena *, PyObject *));
DFH_PY_FUNC(PyInterpreterState *, PyInterpreterState_New, (void));
DFH_PY_FUNC(void, PyInterpreterState_Clear, (PyInterpreterState *));
DFH_PY_FUNC(void, PyInterpreterState_Delete, (PyInterpreterState *));
DFH_PY_FUNC(int, _PyState_AddModule, (PyObject*, struct PyModuleDef*));
DFH_PY_FUNC(int, PyState_AddModule, (PyObject*, struct PyModuleDef*));
DFH_PY_FUNC(int, PyState_RemoveModule, (struct PyModuleDef*));
DFH_PY_FUNC(PyObject*, PyState_FindModule, (struct PyModuleDef*));
DFH_PY_FUNC(void, _PyState_ClearModules, (void));
DFH_PY_FUNC(PyThreadState *, PyThreadState_New, (PyInterpreterState *));
DFH_PY_FUNC(PyThreadState *, _PyThreadState_Prealloc, (PyInterpreterState *));
DFH_PY_FUNC(void, _PyThreadState_Init, (PyThreadState *));
DFH_PY_FUNC(void, PyThreadState_Clear, (PyThreadState *));
DFH_PY_FUNC(void, PyThreadState_Delete, (PyThreadState *));
DFH_PY_FUNC(void, _PyThreadState_DeleteExcept, (PyThreadState *tstate));
DFH_PY_FUNC(void, PyThreadState_DeleteCurrent, (void));
DFH_PY_FUNC(void, _PyGILState_Reinit, (void));
DFH_PY_FUNC(PyThreadState *, PyThreadState_Get, (void));
DFH_PY_FUNC(PyThreadState *, PyThreadState_Swap, (PyThreadState *));
DFH_PY_FUNC(PyObject *, PyThreadState_GetDict, (void));
DFH_PY_FUNC(int, PyThreadState_SetAsyncExc, (long, PyObject *));
DFH_PY_FUNC(PyGILState_STATE, PyGILState_Ensure, (void));
DFH_PY_FUNC(void, PyGILState_Release, (PyGILState_STATE));
DFH_PY_FUNC(PyThreadState *, PyGILState_GetThisThreadState, (void));
DFH_PY_FUNC(int, PyGILState_Check, (void));
DFH_PY_FUNC(PyObject *, _PyThread_CurrentFrames, (void));
DFH_PY_FUNC(PyInterpreterState *, PyInterpreterState_Head, (void));
DFH_PY_FUNC(PyInterpreterState *, PyInterpreterState_Next, (PyInterpreterState *));
DFH_PY_FUNC(PyThreadState *, PyInterpreterState_ThreadHead, (PyInterpreterState *));
DFH_PY_FUNC(PyThreadState *, PyThreadState_Next, (PyThreadState *));
DFH_PY_FUNC(PyObject*, PyType_FromSpec, (PyType_Spec*));
DFH_PY_FUNC(PyObject*, PyType_FromSpecWithBases, (PyType_Spec*, PyObject*));
DFH_PY_FUNC(void*, PyType_GetSlot, (PyTypeObject*, int));
DFH_PY_FUNC(int, PyType_IsSubtype, (PyTypeObject *, PyTypeObject *));
DFH_PY_FUNC(unsigned long, PyType_GetFlags, (PyTypeObject*));
DFH_PY_FUNC(int, PyType_Ready, (PyTypeObject *));
DFH_PY_FUNC(PyObject *, PyType_GenericAlloc, (PyTypeObject *, Py_ssize_t));
DFH_PY_FUNC(PyObject *, PyType_GenericNew, (PyTypeObject *,
                                               PyObject *, PyObject *));
DFH_PY_FUNC(PyObject *, _PyType_Lookup, (PyTypeObject *, PyObject *));
DFH_PY_FUNC(PyObject *, _PyType_LookupId, (PyTypeObject *, _Py_Identifier *));
DFH_PY_FUNC(PyObject *, _PyObject_LookupSpecial, (PyObject *, _Py_Identifier *));
DFH_PY_FUNC(PyTypeObject *, _PyType_CalculateMetaclass, (PyTypeObject *, PyObject *));
DFH_PY_FUNC(unsigned int, PyType_ClearCache, (void));
DFH_PY_FUNC(void, PyType_Modified, (PyTypeObject *));
DFH_PY_FUNC(PyObject *, _PyType_GetDocFromInternalDoc, (const char *, const char *));
DFH_PY_FUNC(PyObject *, _PyType_GetTextSignatureFromInternalDoc, (const char *, const char *));
DFH_PY_FUNC(int, PyObject_Print, (PyObject *, FILE *, int));
DFH_PY_FUNC(void, _Py_BreakPoint, (void));
DFH_PY_FUNC(void, _PyObject_Dump, (PyObject *));
DFH_PY_FUNC(PyObject *, PyObject_Repr, (PyObject *));
DFH_PY_FUNC(PyObject *, PyObject_Str, (PyObject *));
DFH_PY_FUNC(PyObject *, PyObject_ASCII, (PyObject *));
DFH_PY_FUNC(PyObject *, PyObject_Bytes, (PyObject *));
DFH_PY_FUNC(PyObject *, PyObject_RichCompare, (PyObject *, PyObject *, int));
DFH_PY_FUNC(int, PyObject_RichCompareBool, (PyObject *, PyObject *, int));
DFH_PY_FUNC(PyObject *, PyObject_GetAttrString, (PyObject *, const char *));
DFH_PY_FUNC(int, PyObject_SetAttrString, (PyObject *, const char *, PyObject *));
DFH_PY_FUNC(int, PyObject_HasAttrString, (PyObject *, const char *));
DFH_PY_FUNC(PyObject *, PyObject_GetAttr, (PyObject *, PyObject *));
DFH_PY_FUNC(int, PyObject_SetAttr, (PyObject *, PyObject *, PyObject *));
DFH_PY_FUNC(int, PyObject_HasAttr, (PyObject *, PyObject *));
DFH_PY_FUNC(int, _PyObject_IsAbstract, (PyObject *));
DFH_PY_FUNC(PyObject *, _PyObject_GetAttrId, (PyObject *, struct _Py_Identifier *));
DFH_PY_FUNC(int, _PyObject_SetAttrId, (PyObject *, struct _Py_Identifier *, PyObject *));
DFH_PY_FUNC(int, _PyObject_HasAttrId, (PyObject *, struct _Py_Identifier *));
DFH_PY_FUNC(PyObject **, _PyObject_GetDictPtr, (PyObject *));
DFH_PY_FUNC(PyObject *, PyObject_SelfIter, (PyObject *));
DFH_PY_FUNC(PyObject *, _PyObject_NextNotImplemented, (PyObject *));
DFH_PY_FUNC(PyObject *, PyObject_GenericGetAttr, (PyObject *, PyObject *));
DFH_PY_FUNC(int, PyObject_GenericSetAttr, (PyObject *,
                                              PyObject *, PyObject *));
DFH_PY_FUNC(int, PyObject_GenericSetDict, (PyObject *, PyObject *, void *));
DFH_PY_FUNC(Py_hash_t, PyObject_Hash, (PyObject *));
DFH_PY_FUNC(Py_hash_t, PyObject_HashNotImplemented, (PyObject *));
DFH_PY_FUNC(int, PyObject_IsTrue, (PyObject *));
DFH_PY_FUNC(int, PyObject_Not, (PyObject *));
DFH_PY_FUNC(int, PyCallable_Check, (PyObject *));
DFH_PY_FUNC(void, PyObject_ClearWeakRefs, (PyObject *));
DFH_PY_FUNC(void, PyObject_CallFinalizer, (PyObject *));
DFH_PY_FUNC(int, PyObject_CallFinalizerFromDealloc, (PyObject *));
DFH_PY_FUNC(PyObject *, _PyObject_GenericGetAttrWithDict, (PyObject *, PyObject *, PyObject *));
DFH_PY_FUNC(int, _PyObject_GenericSetAttrWithDict, (PyObject *, PyObject *,
                                 PyObject *, PyObject *));
DFH_PY_FUNC(PyObject *, _PyObject_GetBuiltin, (const char *name));
DFH_PY_FUNC(PyObject *, PyObject_Dir, (PyObject *));
DFH_PY_FUNC(int, Py_ReprEnter, (PyObject *));
DFH_PY_FUNC(void, Py_ReprLeave, (PyObject *));
DFH_PY_FUNC(void, _Py_NegativeRefcount, (const char *fname,
                                            int lineno, PyObject *op));
DFH_PY_FUNC(PyObject *, _PyDict_Dummy, (void));
DFH_PY_FUNC(Py_ssize_t, _Py_GetRefTotal, (void));
DFH_PY_FUNC(void, inc_count, (PyTypeObject *));
DFH_PY_FUNC(void, dec_count, (PyTypeObject *));
DFH_PY_FUNC(void, _Py_NewReference, (PyObject *));
DFH_PY_FUNC(void, _Py_ForgetReference, (PyObject *));
DFH_PY_FUNC(void, _Py_Dealloc, (PyObject *));
DFH_PY_FUNC(void, _Py_PrintReferences, (FILE *));
DFH_PY_FUNC(void, _Py_PrintReferenceAddresses, (FILE *));
DFH_PY_FUNC(void, _Py_AddToAllObjects, (PyObject *, int force));
DFH_PY_FUNC(void, Py_IncRef, (PyObject *));
DFH_PY_FUNC(void, Py_DecRef, (PyObject *));
DFH_PY_FUNC(void, _PyTrash_deposit_object, (PyObject*));
DFH_PY_FUNC(void, _PyTrash_destroy_chain, (void));
DFH_PY_FUNC(void, _PyTrash_thread_deposit_object, (PyObject*));
DFH_PY_FUNC(void, _PyTrash_thread_destroy_chain, (void));
DFH_PY_FUNC(void, _PyDebugAllocatorStats, (FILE *out, const char *block_name, int num_blocks,
                       size_t sizeof_block));
DFH_PY_FUNC(void, _PyObject_DebugTypeStats, (FILE *out));
