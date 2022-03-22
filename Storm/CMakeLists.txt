cmake_minimum_required (VERSION 3.8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "")

add_library(storm SHARED storm.cpp storm.def)
