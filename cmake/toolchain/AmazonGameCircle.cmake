if (MENCUS_USE_AMAZON_GAME_CIRCLE) 
  set(MENCUS_AMAZON_GAME_CIRCLE_ANDROID_MK 
"include $(CLEAR_VARS)
LOCAL_MODULE := AmazonGameJni
LOCAL_SRC_FILES := libs/libAmazonGamesJni.so
include $(PREBUILT_SHARED_LIBRARY)")

  set(MENCUS_AMAZON_GAME_CIRCLE_LIBRARY "AmazonGamesJni")
endif()