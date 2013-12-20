#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#include <Ogre.h>

class CMap;

class CMapManager {
private:
  Ogre::SceneManager *m_pSceneManager;
  CMap *m_pCurrentMap;
public:
  CMapManager(Ogre::SceneManager *pSceneManager);
  ~CMapManager();
};

#endif
