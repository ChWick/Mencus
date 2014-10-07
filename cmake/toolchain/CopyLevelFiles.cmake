if (NOT LEVEL_TARGET_PATH)
  message(FATAL_ERROR "LEVEL_TARGET_PATH not set")
endif()

set(LEVEL_TARGET_DIR "${LEVEL_TARGET_PATH}/user/")
set(LEVEL_SRC_DIR "${CMAKE_SOURCE_DIR}/level/user/")

# generate map packs

if (UNIX)
EXECUTE_PROCESS(COMMAND python generate_packs.py WORKING_DIRECTORY ${LEVEL_SRC_DIR})
endif()

set(LEVELS_LIST "")

function(copy_level name tutorial)
  file(COPY "${LEVEL_SRC_DIR}${name}.zip" DESTINATION "${LEVEL_TARGET_DIR}")

  set(ENTRY "  <level file=\"${name}\"")
  if (${tutorial} STREQUAL "true") 
    set(ENTRY "${ENTRY} tutorial=\"true\"")
  endif()
  set(LEVELS_LIST "${LEVELS_LIST}${ENTRY}/>\n" PARENT_SCOPE)
  
endfunction()

function(copy_renamed_level src target tutorial)
  configure_file("${LEVEL_SRC_DIR}${src}.zip" "${LEVEL_TARGET_DIR}${target}.zip" COPYONLY)

  set(ENTRY "  <level file=\"${target}\"")
  if (${tutorial} STREQUAL "true") 
    set(ENTRY "${ENTRY} tutorial=\"true\"")
  endif()
  set(LEVELS_LIST "${LEVELS_LIST}${ENTRY}/>\n" PARENT_SCOPE)

endfunction()

if (MENCUS_ENABLE_INPUT_TOUCH EQUAL 1)
  message(STATUS "Copying touch tutorials")
  copy_level("Tutorial_1" true)
  copy_level("Tutorial_2" true)
  copy_level("Tutorial_3" true)
  copy_level("Tutorial_4" true)
  copy_level("Tutorial_5" true)
else()
  message(STATUS "Copying keyboard tutorials")
  copy_renamed_level("Tutorial_1_input_keyboard" "Tutorial_1" true)
  copy_renamed_level("Tutorial_2_input_keyboard" "Tutorial_2" true)
  copy_renamed_level("Tutorial_3_input_keyboard" "Tutorial_3" true)
  copy_renamed_level("Tutorial_4_input_keyboard" "Tutorial_4" true)
  copy_renamed_level("Tutorial_5_input_keyboard" "Tutorial_5" true)
endif()

copy_level("Simple_Pathway" false)
copy_level("Adventure" false)
copy_level("Labyrinth" false)
copy_level("Fly_High" false)

if (MENCUS_ENABLE_INPUT_TOUCH EQUAL 1)
  copy_level("Tutorial_6" true)
else()
  copy_renamed_level("Tutorial_6_input_keyboard" "Tutorial_6" true)
endif()

copy_level("Castle_1" false)
copy_level("Escape" false)
copy_level("Cave_Passage_1" false)
copy_level("TreeOfLife" false)
copy_level("Castle_2" false)
copy_level("Cave_Passage_2" false)

set(LEVELS_LIST "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<levels>\n${LEVELS_LIST}</levels>")
file(WRITE "${LEVEL_TARGET_PATH}level_list.xml" ${LEVELS_LIST})
message(STATUS "Wrote level lists file")