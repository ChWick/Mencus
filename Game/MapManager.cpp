#include "MapManager.hpp"
#include "Map.hpp"

CMapManager::CMapManager(Ogre::SceneManager *pSceneManager) 
  : m_pSceneManager(pSceneManager),
    m_pCurrentMap(NULL) {
  m_pCurrentMap = new CMap(m_pSceneManager);
}
CMapManager::~CMapManager() {
  if (m_pCurrentMap) {delete m_pCurrentMap; m_pCurrentMap = NULL;}
}
