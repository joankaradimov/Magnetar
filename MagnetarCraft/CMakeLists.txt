add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

if (MSVC)
	add_link_options ("/INCREMENTAL:NO") # MSVC can be buggy https://stackoverflow.com/a/2487756/7913479
	add_link_options ("/FIXED")
	add_link_options ("/BASE:0x200000")
endif ()

add_library(MagnetarCraftObject OBJECT
	src/patching/BasePatch.cpp
	src/patching/CallSitePatch.cpp
	src/patching/FailStubPatch.cpp
	src/patching/FunctionPatch.cpp
	src/patching/NopPatch.cpp
	src/starcraft_exe/offsets.cpp
	src/file_info.cpp
	src/MagnetarCraft.cpp
	src/starcraft_executable.cpp
	src/starcraft.cpp
)

add_subdirectory(src)

include_directories("./include")
include_directories(${YAML_CPP_INCLUDE_DIRS})
include_directories(${MAGNETORM_INCLUDE_DIRS})
include_directories("${sol2_SOURCE_DIR}/include")
include_directories("${luajit_SOURCE_DIR}/src")

target_link_libraries(MagnetarCraftObject PRIVATE MemoryModule)
target_link_libraries(MagnetarCraftObject PRIVATE magnetorm)
target_link_libraries(MagnetarCraftObject PRIVATE "yaml-cpp")
target_link_libraries(MagnetarCraftObject PRIVATE libluajit)
target_link_libraries(MagnetarCraftObject PRIVATE "ddraw" "dsound" "version" "imm32")
