#include "AdDisplayManager.hpp"
#include <OgrePlatform.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android/Android.hpp"
#endif

void CAdDisplayManager::showAdPopup() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  OgreAndroidBridge::showAdPopup();
#endif
}
