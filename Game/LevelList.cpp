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

#include "LevelList.hpp"
#include "OgreLogManager.h"
#include "OgreResourceGroupManager.h"
#include "MapInfo.hpp"
#include "OgreStringConverter.h"
#include "XMLHelper.hpp"

using namespace XMLHelper;
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
    info.bTutorial = BoolAttribute(pLevel, "tutorial", false);
    
    m_lLevelInfoList.push_back(info);
  }
  
  Ogre::LogManager::getSingleton().logMessage("Loading level list finished and read " + Ogre::StringConverter::toString(m_lLevelInfoList.size()) + " levels.");
}
