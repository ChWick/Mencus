#include "MapPack.hpp"
#include "MapInfo.hpp"
#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>

using namespace std;

CMapPack::CMapPack(const std::string &sMapName) 
  : XMLResources::CManager(sMapName, "res/", false),
    m_sMapName(sMapName),
    m_bInitialised(false) {

  init();
}
CMapPack::CMapPack(shared_ptr<CMapInfo> pInfo) 
  : XMLResources::CManager(pInfo->getFileName(), "res/", false),
    m_sMapName(pInfo->getFileName()),
    m_bInitialised(false),
    m_pMapInfo(pInfo) {
  
  init();
}
void CMapPack::init() {
  if (m_bInitialised) {return;}

  Ogre::LogManager::getSingleton().logMessage("Creating map pack for " + m_sMapName);
  Ogre::ResourceGroupManager::getSingleton().createResourceGroup(m_sMapName);
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation("../level/user/" + m_sMapName + ".zip", "Zip", m_sMapName, true);
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(m_sMapName);

  m_bInitialised = true;
  
  try {
    loadLanguage();
  }
  catch (const Ogre::Exception& e) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, e.getFullDescription());
  }

  if (!m_pMapInfo) {
    m_pMapInfo = shared_ptr<CMapInfo>(new CMapInfo(m_sMapName, m_sMapName));
  }
}
CMapPack::~CMapPack() {
  Ogre::LogManager::getSingleton().logMessage("Deleting map pack for " + m_sMapName);
  m_pMapInfo.reset();
  Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup(m_sMapName);
}
void CMapPack::setMapInfo(shared_ptr<CMapInfo> pMapInfo) {
  m_pMapInfo = pMapInfo;

  m_sMapName = pMapInfo->getFileName();
}
