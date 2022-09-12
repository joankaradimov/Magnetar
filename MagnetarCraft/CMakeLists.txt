set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

if (MSVC)
	add_link_options ("/INCREMENTAL:NO") # MSVC can be buggy https://stackoverflow.com/a/2487756/7913479
	add_link_options ("/FIXED")
	add_link_options ("/BASE:0x200000")
endif ()

add_executable(MagnetarCraft WIN32
	src/patching/BasePatch.cpp
	src/patching/CallSitePatch.cpp
	src/patching/FailStubPatch.cpp
	src/patching/FunctionPatch.cpp
	src/patching/NopPatch.cpp
	src/starcraft_exe/offsets.cpp
	src/file_info.cpp
	src/MagnetarCraft.cpp
	src/starcraft.cpp
)

include_directories("./include")
include_directories(${YAML_CPP_INCLUDE_DIRS})
include_directories(${MAGNETORM_INCLUDE_DIRS})
include_directories("${sol2_SOURCE_DIR}/include")
include_directories("${luajit_SOURCE_DIR}/src")

target_link_libraries(MagnetarCraft PRIVATE MemoryModule)
target_link_libraries(MagnetarCraft PRIVATE magnetorm)
target_link_libraries(MagnetarCraft PRIVATE "yaml-cpp")
target_link_libraries(MagnetarCraft PRIVATE libluajit)
target_link_libraries(MagnetarCraft PRIVATE "ddraw" "dsound" "version" "imm32")
