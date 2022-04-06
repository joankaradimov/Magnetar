FUNCTION(MAKE_COMPRESSOR)
    SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin" )

    add_executable(compressor compressor/compressor.cpp)
    include_directories(${STORM_LIB_INCLUDE_DIRS})
    target_link_libraries(compressor PRIVATE storm)
ENDFUNCTION ()

MAKE_COMPRESSOR()

add_custom_target(MagnetarDat
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/bin/compressor"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_dependencies(MagnetarDat compressor)
