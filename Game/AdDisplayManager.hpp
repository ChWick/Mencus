#ifndef _AD_DISPLAY_MANAGER_HPP_
#define _AD_DISPLAY_MANAGER_HPP_

#include <OgrePlatform.h>

#ifndef NO_ADDS

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#define ADDS_ANDROID
#endif

#endif

class CAdDisplayManager {
public:
  static void showAdPopup();
  static bool adPopupClosed();
};

#endif
