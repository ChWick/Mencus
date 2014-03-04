#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <OgrePlatform.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Ogre", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Ogre", __VA_ARGS__))
#else
#define LOGI(...)
#define LOGW(...)
#endif

#endif
