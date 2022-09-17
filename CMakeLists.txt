cmake_minimum_required (VERSION 3.8)

project ("Magnetar")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

if (MSVC)
    # This is needed for errors to be clickable in the VisualStudio UI
    add_compile_options ("-FC")
endif ()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("MagnetarCraft")
add_subdirectory("MagnetarDat")
add_subdirectory("Magnetorm")
set(TESTSUITE OFF CACHE INTERNAL "")
set(MEMORY_MODULE_EXAMPLES OFF CACHE INTERNAL "")
add_subdirectory("MemoryModule")
add_subdirectory("MpqBuilder")
if (MSVC)
    set(CMAKE_DISABLE_FIND_PACKAGE_BZIP2 TRUE)
    set(CMAKE_DISABLE_FIND_PACKAGE_ZLIB TRUE)
endif ()
add_subdirectory("StormLib")

add_subdirectory("LuaJIT")
add_subdirectory("sol2")

set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "")
add_subdirectory("yaml-cpp")
