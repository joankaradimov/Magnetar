project(MagnetarTest)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

include_directories("../include")
include_directories(${MemoryModule_SOURCE_DIR})
include_directories(${MAGNETORM_INCLUDE_DIRS})

add_executable(MagnetarTest tests.cpp)

include(GoogleTest)
target_link_libraries(MagnetarTest PRIVATE MagnetarCraftObject)
target_link_libraries(MagnetarTest PRIVATE magnetorm)
target_link_libraries(MagnetarTest PRIVATE GTest::gtest_main)

gtest_discover_tests(MagnetarTest DISCOVERY_MODE PRE_TEST)
