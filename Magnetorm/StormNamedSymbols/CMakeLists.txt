cmake_minimum_required (VERSION 3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")

add_library(StormNamedSymbols SHARED storm.cpp storm.def)

set_target_properties(StormNamedSymbols PROPERTIES OUTPUT_NAME "storm")
