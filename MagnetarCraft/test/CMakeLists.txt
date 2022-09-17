project(MagnetarTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

include_directories("../include")

add_executable(MagnetarTest tests.cpp)

target_link_libraries(MagnetarTest PRIVATE MagnetarCraftObject)
