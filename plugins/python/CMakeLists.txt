DFHACK_PLUGIN(python python.cpp)
set(PY_VERSION 3.6.1)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(python ${CMAKE_CURRENT_SOURCE_DIR}/lib/osx64/libpython3.6.dylib)
install(FILES lib/osx64/libpython3.6.dylib
    DESTINATION hack)
install(DIRECTORY hack/
    DESTINATION hack/python)
install(FILES lib/stdlib-${PY_VERSION}.zip
    DESTINATION hack/python/
    RENAME stdlib-python.zip)
install(DIRECTORY lib/osx64/stdlib-dynload-${PY_VERSION}/
    DESTINATION hack/python/stdlib-dynload)
