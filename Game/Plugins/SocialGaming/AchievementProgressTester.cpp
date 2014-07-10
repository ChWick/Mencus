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

#include "AchievementProgressTester.hpp"
#include "SocialGaming.hpp"
#include "SocialGamingConnectionInterface.hpp"
#include "Log.hpp"

using namespace SocialGaming;


void CAchievementProgressTester::update(const SStatistics &stats) {
  if (stats.eMissionState == MissionState::MS_ACCOMPLISHED) {
    LOGV("Level finished in social gaming: %s", stats.sLevelFileName.c_str());
    if (stats.sLevelFileName == "Tutorial_1") {
      updateAchievementsProgress(BOOTS_1, 100);
    }
    else if (stats.sLevelFileName == "Tutorial_2") {
      updateAchievementsProgress(BOOTS_2, 100);
    }
    else if (stats.sLevelFileName == "Tutorial_3") {
      updateAchievementsProgress(BOOTS_3, 100);
    }
    else if (stats.sLevelFileName == "Tutorial_4") {
      updateAchievementsProgress(BOOTS_4, 100);
    }
    else if (stats.sLevelFileName == "Tutorial_5") {
      updateAchievementsProgress(BOOTS_5, 100);
    }
    else if (stats.sLevelFileName == "Tutorial_6") {
      updateAchievementsProgress(BOOTS_6, 100);
    }
  }
}

void CAchievementProgressTester::updateAchievementsProgress(EAchievements achievement, float fPercentComplete) {  
  m_SocialGaming.getConnection()->updateAchievementsProgress(achievement, fPercentComplete);
}
