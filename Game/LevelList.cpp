#include "LevelList.hpp"
#include "OgreLogManager.h"
#include "OgreResourceGroupManager.h"
#include "MapInfo.hpp"
#include "OgreStringConverter.h"

using namespace tinyxml2;

CLevelList::CLevelList() {
}
void CLevelList::clear() {
  Ogre::LogManager::getSingleton().logMessage("Clearing the level info list");
  m_lLevelInfoList.clear();
}
void CLevelList::load() {
  Ogre::LogManager::getSingleton().logMessage("Loading level list");
  clear();
  
  Ogre::DataStreamPtr data = Ogre::ResourceGroupManager::getSingleton().openResource("level_list.xml", "level_user", false);
  if (data.isNull()) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "level_list.xml could not be found");
  }

  XMLDocument doc;
  doc.Parse(data->getAsString().c_str());
  if (doc.Error()) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "error while parsing level_list.xml");
    return;
  }

  XMLElement *pLevels = doc.FirstChildElement();
  if (!pLevels) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "root node not found in level_list.xml");
    return;
  }
  Ogre::LogManager::getSingleton().logMessage("Parsing level files");

  for (XMLElement *pLevel = pLevels->FirstChildElement(); pLevel; pLevel = pLevel->NextSiblingElement()) {
    // parse the single maps
    if (!pLevel->Attribute("file")) {
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Level has not file attribute");
      continue;
    }

    SLevelInfo info;
    info.sLevelFileName = pLevel->Attribute("file");

    try {
      CMapInfo mapInfo(info.sLevelFileName, "level_user");
      info.sLevelName = mapInfo.getName();
      info.sDifficulty = mapInfo.getDifficultyAsString();
      info.sLevelDescription = mapInfo.getDescription();
      info.sFullInfoText = mapInfo.generateInfoText();
      info.bTutorial = mapInfo.isTutorial();
    }
    catch (...) {
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Error while parsing the level: " + info.sLevelFileName);
      continue;
    }
    
    m_lLevelInfoList.push_back(info);
  }
  
  Ogre::LogManager::getSingleton().logMessage("Loading level list finished and read " + Ogre::StringConverter::toString(m_lLevelInfoList.size()) + " levels.");
}
