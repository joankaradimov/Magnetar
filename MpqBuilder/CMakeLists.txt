FUNCTION(MAKE_MPQ_BUILDER)
    SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" )

    project(MpqBuilder)
    add_executable(mpqbuilder main.cpp)
    include_directories(${STORM_LIB_INCLUDE_DIRS})
    target_link_libraries(mpqbuilder PRIVATE storm)
ENDFUNCTION ()

MAKE_MPQ_BUILDER()
