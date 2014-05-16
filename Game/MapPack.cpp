/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

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
  Ogre::FileInfoListPtr files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("level_user", m_sMapName + ".zip");
  if (files->size() == 0) {
    throw Ogre::Exception(0, "Map zip file not found", __FILE__);
  }
  Ogre::String sZipFile = files->front().archive->getName() + "/" + files->front().filename;
  Ogre::LogManager::getSingleton().logMessage("  adding zip from  " + sZipFile);
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(sZipFile, "APKZip", m_sMapName, true);
#else
  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(sZipFile, "Zip", m_sMapName, true);
#endif
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
std::string CMapPack::generateInfoText() const {
  std::string sMapName("unset");
  std::string sMapDescription("unset");
  try {
    sMapName = getString("map_name");
  }
  catch (...) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
						"Map name not set");
  }
  try {
    sMapDescription = getString("map_description");
  }
  catch (...) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
						"Map description not set");
  }
  // load name, difficulty, description from language files
  return XMLResources::GLOBAL.getString("Name") + ": "
    + sMapName + "\n" + XMLResources::GLOBAL.getString("Difficulty")
    + ": " + XMLResources::GLOBAL.getString(m_pMapInfo->getDifficultyAsString())
    + "\n" + XMLResources::GLOBAL.getString("Description") + ": "
    + sMapDescription;
}
