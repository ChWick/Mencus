#include "Manager.hpp"

namespace XMLResources {
  std::string CManager::LANGUAGE_CODE;

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  CManager GLOBAL("", "language/", false);
#else
  CManager GLOBAL("", "../language/", false);
#endif
};
