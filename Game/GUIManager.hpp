#ifndef GUIMANAGER_HPP
#define GUIMANAGER_HPP

#include <OgreSingleton.h>

class CGUIManager : public Ogre::Singleton<CGUIManager> {
public:
  static CGUIManager& getSingleton(void);
  static CGUIManager* getSingletonPtr(void);
  
  CGUIManager();
  ~CGUIManager();
  
  void update(Ogre::Real tpf);
};

#endif
