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

#include "DefaultGameData.hpp"
#include <tinyxml2.h>
#include "FileManager.hpp"
#include "Log.hpp"
#include "XMLHelper.hpp"

using namespace SocialGaming;
using namespace MissionState;
using namespace tinyxml2;
using namespace XMLHelper;


CDefaultGameData::CDefaultGameData() 
  : m_sFileName("game_data.xml") {
  read();
}
CDefaultGameData::~CDefaultGameData() {
  save();
}
CLevelList CDefaultGameData::getLevelList() {
  return m_LevelList;
}
void CDefaultGameData::setMissionStateOfLevel(EMissionState eMissionState,
					      const std::string &sLevelName) {
  for (SLevelData &data : m_LevelList) {
    if (data.sLevelName == sLevelName) {
      data.eMissionState = eMissionState;
      return;
    }
  }

  SLevelData data;
  data.eMissionState = eMissionState;
  data.sLevelName = sLevelName;
  // add new
  m_LevelList.push_back(data);
}

void CDefaultGameData::save() const {
  LOGI("Saving game data to file");
  XMLDocument doc;
  XMLElement *pRoot = doc.NewElement("game_data");
  doc.InsertFirstChild(pRoot);

  XMLElement *pLevelData = doc.NewElement("level_data");
  pRoot->InsertEndChild(pLevelData);

  for (const SLevelData &data : m_LevelList) {
    XMLElement *pSingleLevelData = doc.NewElement("level");
    pLevelData->InsertEndChild(pSingleLevelData);

    SetAttribute(pSingleLevelData, "level", data.sLevelName);
    SetAttribute(pSingleLevelData, "mission_state", toString(data.eMissionState));
  }

  if (doc.SaveFile(m_sFileName.c_str())) {
    LOGW("Error on saving game data file");
  }
}

void CDefaultGameData::read() {
  LOGI("Reading game data file");

  XMLDocument doc;
  doc.LoadFile(CFileManager::getValidPath(m_sFileName,
					  CFileManager::SL_INTERNAL).c_str());
  if (doc.Error()) {
    LOGW("Error on parsing game data: %s", doc.GetErrorStr1());
    return;
  }

  XMLElement *pRoot = doc.FirstChildElement();
  if (!pRoot) {
    // empty file
    LOGI("Empty game data file. Returning.");
    return;
  }
  for (XMLElement *pData = pRoot->FirstChildElement(); pData; pData = pData->NextSiblingElement()) {
    if (strcmp(pData->Value(), "level_data") == 0) {
      for (XMLElement *pLevel = pData->FirstChildElement(); pLevel; pLevel = pLevel->NextSiblingElement()) {
	SLevelData data;
	data.sLevelName = Attribute(pLevel, "level");
	if (data.sLevelName.size() == 0) {
	  continue;
	}
	data.eMissionState = parseMissionState(Attribute(pLevel, "mission_state"));
	// this is for compability issue
	if (data.sLevelName.rfind(".xml") != std::string::npos) {
	  // cut
	  data.sLevelName = data.sLevelName.substr(0, data.sLevelName.size() - 4);
	}

	m_LevelList.push_back(data);
      }
    }
  }

  LOGI("Read %d levels from game data", (int)m_LevelList.size());
}
