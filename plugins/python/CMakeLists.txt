DFHACK_PLUGIN(python python.cpp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(python ${CMAKE_CURRENT_SOURCE_DIR}/lib/osx64/libpython3.6.dylib)
install(DIRECTORY hack/
    DESTINATION hack/python)
install(FILES lib/stdlib-3.6.1.zip
    DESTINATION hack/python/
    RENAME stdlib-python.zip)
install(FILES lib/osx64/stdlib-dynload-3.6.1.zip
    DESTINATION hack/python/
    RENAME stdlib-dynload.zip)
install(DIRECTORY lib/osx64/zlib-3.6.1/
    DESTINATION hack/python/zlib)
