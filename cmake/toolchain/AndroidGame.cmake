if (ANDROID)  
  # create the GlobalBuildDefines file
  set (MENCUS_CONFIG_OUT "@CMAKE_BINARY_DIR@/include")
  set(MENCUS_ENABLE_MAP_EDITOR 0)
  set(MENCUS_ENABLE_SHADER 1)
  include(toolchain/CreateGlobalDefines)

  if(APPLE OR WIN32)
    SET(ANDROID_EXECUTABLE "android")
    SET(NDK_BUILD_EXECUTABLE "ndk-build")
  else()
    SET(ANDROID_EXECUTABLE "$ENV{ANDROID_SDK}/tools/android")
    SET(NDK_BUILD_EXECUTABLE "$ENV{ANDROID_NDK}/ndk-build")
  endif()

  SET(ANDROID_MOD_NAME "Mencus")
  SET(DEPENDENCIES_LDLIBS "")
  SET(SOURCE_DIR "@CMAKE_SOURCE_DIR@/Game")
  FILE(GLOB_RECURSE GAME_SOURCE_FILES "Game/*.cpp")
  STRING (REPLACE ";" "\nLOCAL_SRC_FILES += " GAME_SOURCE_FILES "${GAME_SOURCE_FILES}")
  SET(SOURCE_FILES ${GAME_SOURCE_FILES})
  SET(ANT_EXECUTABLE "ant")
  
  if(${ANDROID_NATIVE_API_LEVEL} LESS 14)
    MATH(EXPR ANDROID_SDK_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}+1")
  else()
    SET(ANDROID_SDK_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}")
    #SET(SCREEN_SIZE "|screenSize")
  endif()
  
  if(OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER)
    SET(GLES_OPTIMISER "-lglsl_optimizer -lmesa -lglcpp-library")
  endif()

  SET(ANDROID_TARGET "android-${ANDROID_SDK_API_LEVEL}")
  
  SET(NDKOUT "${CMAKE_BINARY_DIR}/Game")
  file(MAKE_DIRECTORY "${NDKOUT}")
  file(MAKE_DIRECTORY "${NDKOUT}/jni")
  file(MAKE_DIRECTORY "${NDKOUT}/assets")
  file(MAKE_DIRECTORY "${NDKOUT}/assets/gfx")
  file(MAKE_DIRECTORY "${NDKOUT}/assets/packs")
  file(MAKE_DIRECTORY "${NDKOUT}/src")
  file(MAKE_DIRECTORY "${NDKOUT}/level")
  file(MAKE_DIRECTORY "${NDKOUT}/level/user")
  
  # we need network access if using the amazon game circle
  if (MENCUS_ENABLE_ADS OR MENCUS_USE_AMAZON_GAME_CIRCLE) 
    SET(MENCUS_ANDROID_PERMISSIONS
      "<uses-permission android:name=\"android.permission.INTERNET\" /> 
  <!-- Used to avoid sending an ad request if there is no connectivity. -->
  <uses-permission android:name=\"android.permission.ACCESS_NETWORK_STATE\" />")
  endif()

  file(WRITE "${NDKOUT}/default.properties" "target=${ANDROID_TARGET}")
  file(WRITE "${NDKOUT}/jni/Application.mk" "APP_ABI := ${ANDROID_ABI}\nAPP_STL := gnustl_static ")
  configure_file("${MENCUS_TEMPLATES_DIR}/AndroidManifest.xml.in" "${NDKOUT}/AndroidManifest.xml" @ONLY)

  configure_file("${MENCUS_TEMPLATES_DIR}/Android.mk.in" "${NDKOUT}/jni/Android.mk" @ONLY)

  # resources file
  SET(RESOURCES_MINIMAL "Minimal")
  SET(RESOURCES_USING_APK "APK")
  SET(RESOURCES_PREFIX "/")

  configure_file("${MENCUS_TEMPLATES_DIR}/resources.cfg.in" "${NDKOUT}/assets/resources.cfg" @ONLY)

  # copy assets files
  file(COPY "${CMAKE_SOURCE_DIR}/android/assets" DESTINATION "${NDKOUT}")
  file(COPY "${CMAKE_SOURCE_DIR}/cegui" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/gfx/overlay" DESTINATION "${NDKOUT}/assets/gfx")
  file(GLOB gfxPacks "${CMAKE_SOURCE_DIR}/gfx/*.zip")
  file(COPY ${gfxPacks} DESTINATION "${NDKOUT}/assets/gfx")

  # copy the level files (map packs)
  set(LEVEL_TARGET_PATH "${NDKOUT}/assets/level/")
  include(toolchain/CopyLevelFiles)

  # copy materials/overlays/credits/...
  file(COPY "${CMAKE_SOURCE_DIR}/materials" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/overlays" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/credits" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/packs/SdkTraysMinimal.zip" DESTINATION "${NDKOUT}/assets/packs")
  file(COPY "${CMAKE_SOURCE_DIR}/RTShaderLib" DESTINATION "${NDKOUT}/assets")

  # copy language files
  file(COPY "${CMAKE_SOURCE_DIR}/language" DESTINATION "${NDKOUT}/assets")

  # copy resource files (icons e.g.)
  file(COPY "${CMAKE_SOURCE_DIR}/android/res" DESTINATION "${NDKOUT}")

  # copy the java src code files
  file(COPY "${CMAKE_SOURCE_DIR}/android/src" DESTINATION "${NDKOUT}")

  # copy the amazon files if needed
  if (MENCUS_USE_AMAZON_GAME_CIRCLE) 
    file(COPY "${CMAKE_SOURCE_DIR}/android/jni" DESTINATION "${NDKOUT}")
  endif()

  add_custom_command(
    TARGET Game
    POST_BUILD
    COMMAND ${ANDROID_EXECUTABLE} update project --target ${ANDROID_TARGET} --path "${NDKOUT}"
    WORKING_DIRECTORY ${NDKOUT}
    )

endif()
