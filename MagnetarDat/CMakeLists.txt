set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Magnetar)

add_custom_target(MagnetarDat
  COMMAND set "PATH=${StormLib_BINARY_DIR};%PATH%" # TODO handle Unix
  COMMAND "${MpqBuilder_BINARY_DIR}/mpqbuilder" "${CMAKE_CURRENT_SOURCE_DIR}"
  WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
  DEPENDS mpqbuilder
)
