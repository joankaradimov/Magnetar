add_custom_command(OUTPUT "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MagnetarDat.mpq"
  COMMAND "${MpqBuilder_BINARY_DIR}/mpqbuilder" "${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  DEPENDS mpqbuilder)

add_custom_target(MagnetarDat ALL
  DEPENDS "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MagnetarDat.mpq"
)
