project(Game)


SET(SOURCE_DIR "@CMAKE_SOURCE_DIR@/Game")

file(GLOB_RECURSE Game_source_files "@SOURCE_DIR@/*.cpp")
#file(GLOB Game_source_event_files Events/*.cpp)
#file(GLOB Game_source_xmlresources_files XMLResources/*.cpp)
#file(GLOB Game_source_xmlresources_files XMLResources/*.cpp)
file(GLOB_RECURSE Game_include_files "@SOURCE_DIR@/*.hpp")

ADD_DEFINITIONS(-std=c++11 -Wall)


set (MENCUS_CONFIG_OUT "@CMAKE_BINARY_DIR@/include")
include(toolchain/CreateGlobalDefines)

set(Game_include_directories
    ${MENCUS_CONFIG_OUT}
    ..
    .
    ${OIS_INCLUDE_DIR}
    ${OGRE_INCLUDE_DIR}
    ${CEGUI_INCLUDE_DIR}
    ${TINYXML2_INCLUDE_DIR}
)
set(Game_link_libraries
    ${OIS_LIBRARIES}
    ${OGRE_LIBRARIES}
    ${CEGUI_LIBRARIES}
    ${TINYXML2_LIBRARIES}
)

set(Game_link_libraries
  ${Game_link_libraries}
  boost_system
  boost_thread
  boost_regex
  pthread)


set(EXECUTABLE_OUTPUT_PATH ../bin)

include_directories(${Game_include_directories})
add_executable(Game ${Game_source_files} ${Game_source_event_files} ${Game_source_xmlresources_files} ${Game_include_files})
target_link_libraries(Game ${Game_link_libraries})

# create plugins file
if (UNIX)
  find_path(PLUGIN_OPEN_GL_LIB NAMES RenderSystem_GL.so PATH_SUFFIXES Plugins Ogre OGRE lib/OGRE lib/OGRE/Plugins)
  find_package_handle_standard_args(PLUGIN_OPEN_GL DEFAULT_MSG PLUGIN_OPEN_GL_LIB)
  find_path(PLUGIN_OPEN_GLES2_LIB NAMES RenderSystem_GLES2.so PATH_SUFFIXES Plugins Ogre OGRE lib/OGRE lib/OGRE/Plugins)
  find_package_handle_standard_args(PLUGIN_OPEN_GLES2 DEFAULT_MSG PLUGIN_OPEN_GLES2_LIB)

  
  set(PLUGINS "")
  if (PLUGIN_OPEN_GL_FOUND)
    set(PLUGIN_FOLDER ${PLUGIN_OPEN_GL_LIB})
    set(PLUGINS "${PLUGINS}Plugin=RenderSystem_GL\n")
  endif()
  if (PLUGIN_OPEN_GLES2_FOUND)
    set(PLUGIN_FOLDER ${PLUGIN_OPEN_GLES2_LIB})
    set(PLUGINS "${PLUGINS}Plugin=RenderSystem_GLES2\n")
  endif()
else()
  set(PLUGIN_FOLDER "${CMAKE_SOURCE_DIR}/plugins")
  set(PLUGINS "Plugin=RenderSystem_D9")
endif()

configure_file("${MENCUS_TEMPLATES_DIR}/plugins.cfg.in" "${EXECUTABLE_OUTPUT_PATH}/plugins.cfg" @ONLY)


# create resources file
set(RESOURCES_PREFIX "..")
set(RESOURCES_USING_APK "")
configure_file("${MENCUS_TEMPLATES_DIR}/resources.cfg.in" "${EXECUTABLE_OUTPUT_PATH}/resources.cfg" @ONLY)