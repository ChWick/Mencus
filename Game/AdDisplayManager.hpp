#ifndef _AD_DISPLAY_MANAGER_HPP_
#define _AD_DISPLAY_MANAGER_HPP_

#include <OgrePlatform.h>
#include "GlobalBuildDefines.hpp"

#if MENCUS_ENABLE_ADS == 1

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#define ADDS_ANDROID
#endif

#endif

class CAdDisplayManager {
private:
  static int m_iWaitingLoops;
  static int m_iWaitedLoops;
public:
  static void setWaitingLoops(int iLoops) {m_iWaitingLoops = iLoops;}
  static void showAdPopup();
  static bool adPopupClosed();

private:
  static bool isAdPopupClosed();
};

#endif
