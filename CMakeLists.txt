cmake_minimum_required (VERSION 3.9)

include(CheckIPOSupported)

project ("Magnetar")

check_ipo_supported(RESULT result)
if(result)
    cmake_policy(SET CMP0069 NEW)
    set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

if (MSVC)
    # This is needed for errors to be clickable in the VisualStudio UI
    add_compile_options ("-FC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
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

set(GTEST_CREATE_SHARED_LIBRARY TRUE)
set(GTEST_LINKED_AS_SHARED_LIBRARY TRUE)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  cpp-peglib
  GIT_REPOSITORY https://github.com/yhirose/cpp-peglib.git
  GIT_TAG v1.8.5
)
FetchContent_MakeAvailable(cpp-peglib)
