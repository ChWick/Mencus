################################################################################
# Custom cmake module for CEGUI to find OGRE
################################################################################
include(FindPackageHandleStandardArgs)

find_path(OGRE_H_PATH NAMES Ogre.h PATH_SUFFIXES OGRE PATHS ${OGRE_SDK_ROOT}/include)
find_path(OGRE_OVERLAY_PATH NAMES OgreOverlaySystem.h PATH_SUFFIXES OGRE OGRE/Overlay PATHS ${OGRE_SDK_ROOT}/include)

if (${MENCUS_STATIC_DEPENDENCIES})
  find_library(OGRE_LIB NAMES OgreMainStatic)
  find_library(OGRE_OVERLAY_LIB NAMES OgreOverlayStatic)

  find_path(OGRE_PLUGIN_PARTICLE_FX_PATH OgreParticleFXPlugin.h PATH_SUFFIXES OGRE/Plugins/ParticleFX)
  find_library(OGRE_PLUGIN_PARTICLE_FX_LIB Plugin_ParticleFXStatic PATH_SUFFIXES OGRE)

  find_path(OGRE_PLUGIN_RENDERSYSTEMS_GLES2_PATH OgreGLES2Plugin.h PATH_SUFFIXES OGRE/RenderSystems/GLES2)
  find_library(OGRE_PLUGIN_RENDER_SYSTEM_GLES2_LIB RenderSystem_GLES2Static PATH_SUFFIXES OGRE)

  find_path(OGRE_RT_SHADER_SYSTEM_PATH OgreRTShaderSystem.h PATH_SUFFIXES OGRE/RTShaderSystem)
  find_library(OGRE_RT_SHADER_SYSTEM_LIB OgreRTShaderSystemStatic)

  find_path(OGRE_DEPS_ZZIP_PATH zconf.h)
  find_library(OGRE_DEPS_ZZIP_LIB zzip)

  find_path(OGRE_DEPS_Z_PATH zlib.h)
  find_library(OGRE_DEPS_Z_LIB zlib)

  find_path(OGRE_DEPS_FREEIMAGE_PATH NAMES FreeImage.h freeimage.h)
  find_library(OGRE_DEPS_FREEIMAGE_LIB NAMES FreeImage freeimage)

  find_path(OGRE_DEPS_FREETYPE_BUILD_PATH NAMES ft2build.h )
  find_path(OGRE_DEPS_FREETYPE_FT_PATH NAMES freetype.h PATH_SUFFIXES freetype)
  find_library(OGRE_DEPS_FREETYPE_LIB NAMES freetype)
  
  mark_as_advanced(
    OGRE_LIB OGRE_H_PATH
    OGRE_OVERLAY_PATH OGRE_OVERLAY_LIB
    OGRE_PLUGIN_PARTICLE_FX_PATH OGRE_PLUGIN_PARTICLE_FX_LIB
    OGRE_PLUGIN_RENDERSYSTEMS_GLES2_PATH OGRE_PLUGIN_RENDER_SYSTEM_GLES2_LIB
    OGRE_RT_SHADER_SYSTEM_PATH OGRE_RT_SHADER_SYSTEM_LIB

    OGRE_DEPS_ZZIP_PATH OGRE_DEPS_ZZIP_LIB
    OGRE_DEPS_Z_PATH OGRE_DEPS_Z_LIB
    OGRE_DEPS_FREEIMAGE_PATH OGRE_DEPS_FREEIMAGE_LIB
    OGRE_DEPS_FREETYPE_BUILD_PATH OGRE_DEPS_FREETYPE_FT_PATH OGRE_DEPS_FREETYPE_LIB
    )

  find_package_handle_standard_args(OGRE DEFAULT_MSG
    OGRE_LIB OGRE_H_PATH
    OGRE_OVERLAY_PATH OGRE_OVERLAY_LIB
    OGRE_PLUGIN_PARTICLE_FX_PATH OGRE_PLUGIN_PARTICLE_FX_LIB
    OGRE_PLUGIN_RENDERSYSTEMS_GLES2_PATH OGRE_PLUGIN_RENDER_SYSTEM_GLES2_LIB
    OGRE_RT_SHADER_SYSTEM_PATH OGRE_RT_SHADER_SYSTEM_LIB

    OGRE_DEPS_ZZIP_PATH OGRE_DEPS_ZZIP_LIB
    OGRE_DEPS_Z_PATH OGRE_DEPS_Z_LIB
    OGRE_DEPS_FREEIMAGE_PATH OGRE_DEPS_FREEIMAGE_LIB
    OGRE_DEPS_FREETYPE_BUILD_PATH OGRE_DEPS_FREETYPE_FT_PATH OGRE_DEPS_FREETYPE_LIB
    )
else()
  find_library(OGRE_LIB NAMES OgreMain PATHS ${OGRE_SDK_ROOT}/lib)
  find_library(OGRE_LIB_DBG NAMES OgreMain_d PATHS ${OGRE_SDK_ROOT}/lib)
  find_library(OGRE_OVERLAY_LIB NAMES OgreOverlay PATHS ${OGRE_SDK_ROOT}/lib)

  mark_as_advanced(OGRE_H_PATH OGRE_LIB OGRE_LIB_DBG)

  find_package_handle_standard_args(OGRE DEFAULT_MSG OGRE_LIB OGRE_H_PATH)
endif()


# set up output vars
if (OGRE_FOUND)
    set (OGRE_OCTREE_INCLUDE_DIR ${OGRE_H_PATH}/Plugins/OctreeSceneManager)
    set (OGRE_OVERLAY_INCLUDE_DIR ${OGRE_H_PATH}/Overlay)
    set (OGRE_INCLUDE_DIR
      ${OGRE_H_PATH}
      ${OGRE_OVERLAY_INCLUDE_DIR})
    set (OGRE_LIBRARIES
      ${OGRE_LIB}
      ${OGRE_OVERLAY_LIB})
    if (OGRE_LIB_DBG)
        set (OGRE_LIBRARIES_DBG ${OGRE_LIB_DBG})

    endif()
else()
    set (OGRE_OCTREE_INCLUDE_DIR)
    set (OGRE_INCLUDE_DIR)
    set (OGRE_LIBRARIES)
    set (OGRE_LIBRARIES_DBG)
endif()


