cmake_minimum_required (VERSION 3.8)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
add_link_options(-DELAYLOAD:storm.dll)

add_subdirectory("StormNamedSymbols")

add_library(magnetorm SHARED dllmain.cpp Magnetorm.def)
target_link_libraries(magnetorm PRIVATE StormNamedSymbols)
target_link_libraries(magnetorm PRIVATE delayimp)

target_include_directories(magnetorm PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
