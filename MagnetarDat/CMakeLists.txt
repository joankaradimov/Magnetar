add_custom_target(MagnetarDat
  COMMAND "${MpqBuilder_BINARY_DIR}/mpqbuilder" "${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_dependencies(MagnetarDat mpqbuilder)
