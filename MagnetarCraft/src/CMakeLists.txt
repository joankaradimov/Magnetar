set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

include_directories("../include")

add_executable(MagnetarCraft WIN32 main.cpp)

target_link_libraries(MagnetarCraft PRIVATE MagnetarCraftObject)
