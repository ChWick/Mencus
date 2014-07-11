if (NOT IMAGESET_TARGET_DIR)
  message(FATAL_ERROR "IMAGESET_TARGET_DIR not set")
endif()

set(IMAGESET_SRC_DIR "${CMAKE_SOURCE_DIR}/ResourcesJar/Buttons/")


function(copy_button_imageset filename)
  configure_file("${IMAGESET_SRC_DIR}${filename}.png" "${IMAGESET_TARGET_DIR}input_buttons.png" COPYONLY)
  configure_file("${IMAGESET_SRC_DIR}${filename}.imageset" "${IMAGESET_TARGET_DIR}input_buttons.imageset")
endfunction()

copy_button_imageset(buttons_keyboard)