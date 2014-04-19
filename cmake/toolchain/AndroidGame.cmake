if (ANDROID)  
  # create the GlobalBuildDefines file
  set (MENCUS_CONFIG_OUT "@CMAKE_BINARY_DIR@/include")
  set(MENCUS_ENABLE_MAP_EDITOR 0)
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
  FILE(GLOB GAME_SOURCE_FILES "Game/*.cpp")
  FILE(GLOB GAME_SOURCE_FILES_EVENTS "Game/Events/*.cpp")
  FILE(GLOB GAME_SOURCE_FILES_XMLRESOURCES "Game/XMLResources/*.cpp")
  STRING (REPLACE ";" "\nLOCAL_SRC_FILES += " GAME_SOURCE_FILES "${GAME_SOURCE_FILES} ${GAME_SOURCE_FILES_EVENTS} ${GAME_SOURCE_FILES_XMLRESOURCES}")
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
  
  file(WRITE "${NDKOUT}/default.properties" "target=${ANDROID_TARGET}")
  file(WRITE "${NDKOUT}/jni/Application.mk" "APP_ABI := ${ANDROID_ABI}\nAPP_STL := gnustl_static ")
  configure_file("${MENCUS_TEMPLATES_DIR}/AndroidManifest.xml.in" "${NDKOUT}/AndroidManifest.xml" @ONLY)

  configure_file("${MENCUS_TEMPLATES_DIR}/Android.mk.in" "${NDKOUT}/jni/Android.mk" @ONLY)

  # resources file
  SET(RESOURCES_MINIMAL "Minimal")
  SET(RESOURCES_USING_APK "APK")
  SET(RESOURCES_PREFIX "")
  configure_file("${MENCUS_TEMPLATES_DIR}/resources.cfg.in" "${NDKOUT}/assets/resources.cfg" @ONLY)

  # copy assets files
  file(COPY "${CMAKE_SOURCE_DIR}/cegui" DESTINATION "${NDKOUT}/assets")
  file(COPY "${CMAKE_SOURCE_DIR}/gfx/overlay" DESTINATION "${NDKOUT}/assets/gfx")
  file(GLOB gfxPacks "${CMAKE_SOURCE_DIR}/gfx/*.zip")
  file(COPY ${gfxPacks} DESTINATION "${NDKOUT}/assets/gfx")
  # file(GLOB levelFiles "${CMAKE_SOURCE_DIR}/level/*.zip")
  # file(COPY ${levelFiles} DESTINATION "${NDKOUT}/assets/level")
  file(COPY "${CMAKE_SOURCE_DIR}/level/level_list.xml" DESTINATION "${NDKOUT}/assets/level")
  file(GLOB MAP_PACKS "${CMAKE_SOURCE_DIR}/level/user/*.zip" PATTERN *Test* EXCLUDE)
  file(COPY ${MAP_PACKS} DESTINATION "${NDKOUT}/assets/level/user")
  # file(COPY "${CMAKE_SOURCE_DIR}/level/user" DESTINATION "${NDKOUT}/assets/level" PATTERN *Test* EXCLUDE)
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

  add_custom_command(
    TARGET Game
    POST_BUILD
    COMMAND ${ANDROID_EXECUTABLE} update project --target ${ANDROID_TARGET} --path "${NDKOUT}"
    WORKING_DIRECTORY ${NDKOUT}
    )

endif()
