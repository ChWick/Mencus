#ifndef _LOG_HPP_
#define _LOG_HPP_

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include <android/log.h>
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, "Ogre", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Ogre", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Ogre", __VA_ARGS__))
#else
#include <stdio.h>
#define LOGV(...) (printf(__VA_ARGS__))
#define LOGI(...) (printf(__VA_ARGS__))
#define LOGW(...) (printf(__VA_ARGS__))
#endif

#endif
