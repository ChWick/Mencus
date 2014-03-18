#include "AdDisplayManager.hpp"
#include "Game.hpp"
#include <OgreLogManager.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android/Android.hpp"
#endif

void CAdDisplayManager::showAdPopup() {
#ifdef ADDS_ANDROID
  OgreAndroidBridge::showAdPopup();
#endif
}
bool CAdDisplayManager::adPopupClosed() {
#ifdef ADDS_ANDROID
  return OgreAndroidBridge::adPopupClosed();
#endif
  return true;
}
