cmake_minimum_required (VERSION 3.8)

project ("Magnetar")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

if (MSVC)
    # This is needed for errors to be clickable in the VisualStudio UI
    add_compile_options ("-FC")
endif ()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("MagnetarCraft")
add_subdirectory("Magnetorm")
set(TESTSUITE OFF CACHE INTERNAL "")
set(MEMORY_MODULE_EXAMPLES OFF CACHE INTERNAL "")
add_subdirectory("MemoryModule")
add_subdirectory("StormNamedSymbols")

set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "")
add_subdirectory("yaml-cpp")
