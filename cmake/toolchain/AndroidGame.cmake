if (ANDROID)
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
  FILE(GLOB GAME_SOURCE_FILES "Game/*.cpp")
  STRING (REPLACE ";" "\nLOCAL_SRC_FILES += " GAME_SOURCE_FILES "${GAME_SOURCE_FILES}")
  SET(SOURCE_FILES ${GAME_SOURCE_FILES})
  SET(ANT_EXECUTABLE "ant")
  
  if(${ANDROID_NATIVE_API_LEVEL} LESS 14)
    MATH(EXPR ANDROID_SDK_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}+1")
  else()
    SET(ANDROID_SDK_API_LEVEL "${ANDROID_NATIVE_API_LEVEL}")
    SET(SCREEN_SIZE "|screenSize")
  endif()
  
  if(OGRE_CONFIG_ENABLE_GLES2_GLSL_OPTIMISER)
    SET(GLES_OPTIMISER "-lglsl_optimizer -lmesa -lglcpp-library")
  endif()

  SET(ANDROID_TARGET "android-${ANDROID_SDK_API_LEVEL}")
  
  SET(NDKOUT "${CMAKE_BINARY_DIR}/Game")
  file(MAKE_DIRECTORY "${NDKOUT}")
  file(MAKE_DIRECTORY "${NDKOUT}/jni")
  file(MAKE_DIRECTORY "${NDKOUT}/assets")
  file(MAKE_DIRECTORY "${NDKOUT}/res")	
  file(MAKE_DIRECTORY "${NDKOUT}/src")
  
  file(WRITE "${NDKOUT}/default.properties" "target=${ANDROID_TARGET}")
  file(WRITE "${NDKOUT}/jni/Application.mk" "APP_ABI := ${ANDROID_ABI}\nAPP_STL := gnustl_static ")
  configure_file("${MENCUS_TEMPLATES_DIR}/AndroidManifest.xml.in" "${NDKOUT}/AndroidManifest.xml" @ONLY)

  configure_file("${MENCUS_TEMPLATES_DIR}/Android.mk.in" "${NDKOUT}/jni/Android.mk" @ONLY)

  # resources file
  SET(RESOURCES_USING_APK "APK")
  SET(RESOURCES_PREFIX "")
  configure_file("${MENCUS_TEMPLATES_DIR}/resources.cfg.in" "${NDKOUT}/assets/resources.cfg" @ONLY)

  # copy resource files
  file(COPY "${CMAKE_SOURCE_DIR}/cegui" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/gfx" DESTINATION "${NDKOUT}/assets" PATTERN "*working_files*" EXCLUDE)
  file(GLOB levelFiles "${CMAKE_SOURCE_DIR}/level/*.zip")
  file(COPY ${levelFiles} DESTINATION "${NDKOUT}/assets/level")
  file(COPY "${CMAKE_SOURCE_DIR}/materials" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/overlays" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/packs" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/RTShaderLib" DESTINATION "${NDKOUT}/assets")
  
  add_custom_command(
    TARGET Game
    POST_BUILD
    COMMAND ${ANDROID_EXECUTABLE} update project --target ${ANDROID_TARGET} --path "${NDKOUT}"
    WORKING_DIRECTORY ${NDKOUT}
    )

endif()