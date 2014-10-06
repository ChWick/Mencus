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

using namespace Whispersync;

SocialGaming::CLevelList CGameData::getLevelList() {
SocialGaming::CLevelList out;
AmazonGames::GameDataMap* pLevelList = AmazonGames::WhispersyncClient::getGameData()->getMap("level_list");
AmazonGames::StringList *pLevelKeys = pLevelList->getMapKeys();
for (int i = 0; i < pLevelKeys->getSize(); i++) {
SocialGaming::SLevelData data;
AmazonGames::GameDataMap *pLevel = pLevelList->getMap(pLevelKeys->get(i));
data.sLevelName = pLevel->getLatestString("level_name")->getValue();
data.eMissionState = static_cast<EMissionState>(pLevel->getHighestNumber("level_state")->asInt());
out.push_back(data);
}
return out;
}

#endif
