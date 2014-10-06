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

#include "WhispersyncGameData.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include "WhispersyncClientInterface.h"
#include "Log.hpp"

using namespace Whispersync;
using namespace AmazonGames;

CGameData::CGameData() {
  WhispersyncClient::setNewCloudDataListener(this);
}

CGameData::~CGameData() {
  save();
}

SocialGaming::CLevelList CGameData::getLevelList() {
  LOGI("Getting level list");
  SocialGaming::CLevelList out;
  AmazonGames::GameDataMap* pGameData = AmazonGames::WhispersyncClient::getGameData();
  LOGV("Accessed game data");
  if (!pGameData) {LOGW("Game data is null!"); return out;}

  AmazonGames::GameDataMap* pLevelList = pGameData->getMap("level_list");
  LOGV("Accessed level_list");
  AmazonGames::StringList *pLevelKeys = pLevelList->getMapKeys();
  for (int i = 0; i < pLevelKeys->getSize(); i++) {
    LOGV("Reading level data for map '%s'", pLevelKeys->get(i));
    SocialGaming::SLevelData data(pLevelKeys->get(i));
    AmazonGames::GameDataMap *pLevel = pLevelList->getMap(pLevelKeys->get(i));
    data.sLevelName = pLevelKeys->get(i);
    data.eMissionState = static_cast<MissionState::EMissionState>(pLevel->getHighestNumber("level_state")->asInt());
    out.push_back(data);
  }
  LOGI("Read %d levels", out.size());
  return out;
}

void CGameData::setMissionStateOfLevel(MissionState::EMissionState eMissionState, const std::string &sLevelName, bool bSkipped) {
  AmazonGames::GameDataMap* pLevelList = AmazonGames::WhispersyncClient::getGameData()->getMap("level_list");
  AmazonGames::StringList *pLevelKeys = pLevelList->getMapKeys();
  AmazonGames::GameDataMap *pLevel = pLevelList->getMap(sLevelName.c_str());
  pLevel->getHighestNumber("level_state")->set(eMissionState);

  save();
}
void CGameData::save() const {
  LOGV("Flushing whispersync client");
  WhispersyncClient::flush();
}

void CGameData::onNewCloudData() {
  LOGI("new cloud data added");
}
#endif
