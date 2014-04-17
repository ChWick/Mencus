#include "Manager.hpp"

namespace XMLResources {
  std::string CManager::LANGUAGE_CODE;

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  CManager GLOBAL("", "", false);
#else
  CManager GLOBAL("", "../", false);
#endif
};
