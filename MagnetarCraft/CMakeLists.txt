add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

if (MSVC)
	add_link_options ("/INCREMENTAL:NO") # MSVC can be buggy https://stackoverflow.com/a/2487756/7913479
	add_link_options ("/FIXED")
	add_link_options ("/BASE:0x140000")
endif ()

add_library(MagnetarCraftObject OBJECT
	src/patching/BasePatch.cpp
	src/patching/CallSitePatch.cpp
	src/patching/FailStubPatch.cpp
	src/patching/FunctionPatch.cpp
	src/patching/NopPatch.cpp
	src/starcraft_exe/offsets.cpp
	src/warcraft2_exe/offsets.cpp
	src/campaign.cpp
	src/CBullet.cpp
	src/CFlingy.cpp
	src/CSprite.cpp
	src/iscript.cpp
	src/file_info.cpp
	src/MagnetarCraft.cpp
	src/race.cpp
	src/starcraft_executable.cpp
	src/starcraft.cpp
	src/glu_campaign.cpp
	src/glu_campaign_exp.cpp
	src/glu_chat.cpp
	src/glu_connect.cpp
	src/glu_custom.cpp
	src/glu_join.cpp
	src/glu_load.cpp
	src/glu_login.cpp
	src/glu_main.cpp
	src/minimap.cpp
	src/glu_ready_room.cpp
	src/glu_score.cpp
	src/glu_title.cpp
	src/video.cpp
	src/warcraft2.cpp
)

add_subdirectory(src)
add_subdirectory(test)

include_directories("./include")
include_directories(${MemoryModule_SOURCE_DIR})
include_directories(${YAML_CPP_INCLUDE_DIRS})
include_directories(${MAGNETORM_INCLUDE_DIRS})
include_directories("${sol2_SOURCE_DIR}/include")
include_directories("${luajit_SOURCE_DIR}/src")

target_link_libraries(MagnetarCraftObject PRIVATE MemoryModule)
target_link_libraries(MagnetarCraftObject PRIVATE magnetorm)
target_link_libraries(MagnetarCraftObject PRIVATE "yaml-cpp")
target_link_libraries(MagnetarCraftObject PRIVATE libluajit)
target_link_libraries(MagnetarCraftObject PRIVATE "ddraw" "dsound" "version" "imm32")

add_dependencies(MagnetarCraftObject MagnetarDat)
