cmake_minimum_required (VERSION 3.8)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_library(magnetorm SHARED dllmain.cpp Magnetorm.def)
target_link_libraries(magnetorm PRIVATE StormNamedSymbols)
