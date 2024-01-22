cmake_minimum_required (VERSION 3.9)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")

add_library(StormNamedSymbols SHARED storm.cpp storm.def)

set_target_properties(StormNamedSymbols PROPERTIES OUTPUT_NAME "storm")
target_link_libraries(StormNamedSymbols PRIVATE "delayimp")
