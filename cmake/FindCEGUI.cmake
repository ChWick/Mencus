################################################################################
# Custom cmake module for zelda to find CEGUI
################################################################################
include(FindPackageHandleStandardArgs)

find_path(CEGUI_H_PATH NAMES CEGUI/CEGUI.h PATH_SUFFIXES cegui-0)

if (${MENCUS_STATIC_DEPENDENCIES})
  find_library(CEGUI_LIB NAMES CEGUIBase-0_Static)
  find_library(CEGUI_OGRE_RENDERER_LIB NAMES CEGUIOgreRenderer-0_Static)
  find_library(CEGUI_CORE_WINDOW_RENDERER_SET_LIB CEGUICoreWindowRendererSet_Static)
  find_library(CEGUI_FREE_IMAGE_IMAGE_CODEC_LIB CEGUIFreeImageImageCodec_Static)
  find_library(CEGUI_TINY_XML_PARSER_LIB CEGUITinyXMLParser_Static)

  find_path(CEGUI_DEPS_TINYXML_PATH NAMES tinyxml.h)
  find_library(CEGUI_DEPS_TINYXML_LIB NAMES tinyxml)
  
  find_path(CEGUI_DEPS_ICONV_PATH NAMES iconv.h)
  find_library(CEGUI_DEPS_ICONV_LIB NAMES iconv)

  find_path(CEGUI_DEPS_PCRE_PATH NAMES pcre.h)
  find_library(CEGUI_DEPS_PCRE_LIB NAMES pcre)

  mark_as_advanced(
    CEGUI_H_PATH CEGUI_LIB CEGUI_OGRE_RENDERER_LIB
    CEGUI_CORE_WINDOW_RENDERER_SET_LIB CEGUI_FREE_IMAGE_IMAGE_CODEC_LIB
    CEGUI_TINY_XML_PARSER_LIB

    CEGUI_DEPS_TINYXML_PATH CEGUI_DEPS_TINYXML_LIB
    CEGUI_DEPS_ICONV_PATH CEGUI_DEPS_ICONV_LIB
    CEGUI_DEPS_PCRE_PATH CEGUI_DEPS_PCRE_LIB
    )
  
  find_package_handle_standard_args(CEGUI DEFAULT_MSG

    CEGUI_H_PATH CEGUI_LIB CEGUI_OGRE_RENDERER_LIB
    CEGUI_CORE_WINDOW_RENDERER_SET_LIB CEGUI_FREE_IMAGE_IMAGE_CODEC_LIB
    CEGUI_TINY_XML_PARSER_LIB
    
    CEGUI_DEPS_TINYXML_PATH CEGUI_DEPS_TINYXML_LIB
    CEGUI_DEPS_ICONV_PATH CEGUI_DEPS_ICONV_LIB
    CEGUI_DEPS_PCRE_PATH CEGUI_DEPS_PCRE_LIB
    )
else()
  find_library(CEGUI_LIB NAMES CEGUIBase-0)
  find_library(CEGUI_OGRE_RENDERER_LIB NAMES CEGUIOgreRenderer-0)
  find_library(CEGUI_LIB_DBG NAMES CEGUIBase-0_d)

  mark_as_advanced(CEGUI_H_PATH CEGUI_LIB CEGUI_OGRE_RENDERER_LIB CEGUI_LIB_DBG)
  find_package_handle_standard_args(CEGUI DEFAULT_MSG CEGUI_LIB CEGUI_OGRE_RENDERER_LIB CEGUI_H_PATH)
endif()


# set up output vars
if (CEGUI_FOUND)
  set (CEGUI_INCLUDE_DIR ${CEGUI_H_PATH})
  set (CEGUI_LIBRARIES ${CEGUI_LIB} ${CEGUI_OGRE_RENDERER_LIB})
  if (CEGUI_LIB_DBG)
    set (CEGUI_LIBRARIES_DBG ${CEGUI_LIB_DBG})
  endif()
else()
    set (CEGUI_INCLUDE_DIR)
    set (CEGUI_LIBRARIES)
    set (CEGUI_LIBRARIES_DBG)
endif()


