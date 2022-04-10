add_custom_target(MagnetarDat
  COMMAND "${MpqBuilder_BINARY_DIR}/mpqbuilder"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_dependencies(MagnetarDat mpqbuilder)
