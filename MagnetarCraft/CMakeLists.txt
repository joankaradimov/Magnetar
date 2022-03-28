add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

if (MSVC)
	add_link_options ("/INCREMENTAL:NO") # MSVC can be buggy https://stackoverflow.com/a/2487756/7913479
	add_link_options ("/FIXED")
	add_link_options ("/BASE:0x300000")
endif ()

add_executable(MagnetarCraft WIN32
	patching/BasePatch.cpp
	patching/CallSitePatch.cpp
	patching/FailStubPatch.cpp
	patching/FunctionPatch.cpp
	patching/NopPatch.cpp
	MagnetarCraft.cpp
	starcraft.cpp
	starcraft_exe.cpp
)

include_directories(${YAML_CPP_INCLUDE_DIRS})

target_link_libraries(MagnetarCraft PRIVATE MemoryModule)
target_link_libraries(MagnetarCraft PRIVATE "yaml-cpp")
target_link_libraries(MagnetarCraft PRIVATE "ddraw" "dsound" "version" "imm32")
