
if (NOT DEFINED MENCUS_ENABLE_MAP_EDITOR)
  set(MENCUS_ENABLE_MAP_EDITOR 1 CACHE INTEGER "Enable the mapeditor")
endif()

if (NOT DEFINED MENCUS_ENABLE_CAMPAIGN)
  set(MENCUS_ENABLE_CAMPAIGN 0 CACHE INTEGER "Enable campaigns")
endif()

if (NOT DEFINED MENCUS_ENABLE_CHEATS)
  set(MENCUS_ENABLE_CHEATS 0 CACHE INTEGER "Enable cheats")
endif()

if (NOT DEFINED MENCUS_ENABLE_INPUT_ALL)
  set(MENCUS_ENABLE_INPUT_ALL 0 CACHE INTEGER "Enable all input sources, for debugging")
endif()

if (NOT DEFINED MENCUS_ENABLE_DEBUG_DEFINES)
  set(MENCUS_ENABLE_DEBUG_DEFINES 0 CACHE INTEGER "Enable debug defines, for displaying debug output")
endif()

if (NOT DEFINED MENCUS_ENABLE_SHADER)
  set(MENCUS_ENABLE_SHADER 0 CACHE INTEGER "Use shaders (required for GLSLES)")
endif()

if (NOT DEFINED MENCUS_ASSERT_MODE)
  set(MENCUS_ASSERT_MODE 1 CACHE INTEGER "0: no asserts, 1: use asserts")
endif()

if (NOT DEFINED MENCUS_CONFIG_OUT) 
  Message("Please set the MENCUS_CONFIG_OUT variable to an apropriate path and add it to the include path")
else()
  file(MAKE_DIRECTORY ${MENCUS_CONFIG_OUT})

  configure_file("${MENCUS_TEMPLATES_DIR}/GlobalBuildDefines.hpp.in" "${MENCUS_CONFIG_OUT}/GlobalBuildDefines.hpp" @ONLY)


endif()