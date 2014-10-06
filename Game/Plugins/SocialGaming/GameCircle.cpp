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


#include "GameCircle.hpp"
#include "Sleep.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include "Log.hpp"


using namespace AmazonGames;
using namespace SocialGaming;
using namespace GameCircle;


// setup achievements map
std::string CConnectionInterface::GAME_CIRCLE_ACHIEVEMENTS_MAP[SocialGaming::ACHIEVEMENT_COUNT] = {
  "debug",			// DEBUG_ACHIEVEMENT
  "boot_1",			// BOOTS_1
  "boot_2",			// BOOTS_2
  "boot_3",			// BOOTS_3
  "boot_4",			// BOOTS_4
  "boot_5",			// BOOTS_5
  "boot_6"			// BOOTS_6
};

std::string CConnectionInterface::GAME_CIRCLE_LEADERBOARDS_MAP[SocialGaming::LEADERBOARDS_COUNT] {
  "global_ranking"
};

CConnectionInterface::CConnectionInterface() 
  : CSocialGamingConnectionInterface("Amazon GameCircle",
				     "AmazonGameCircle") {
}

bool CConnectionInterface::init() {
  PlayerClientInterface::setSignedInStateChangedListener(this);
  return PlayerClientInterface::isSignedIn();
}

void CConnectionInterface::loadAchievements() {
  LOGV("Before load achievements progress");
  AchievementsClientInterface::getAchievements(this);
  LOGV("After load achievements progress");
}

void CConnectionInterface::updateLeaderboardValue(SocialGaming::ELeaderboards leaderboard,
						  long long llValue) {
  LOGV("Before update leaderboard '%s' value '%lld'", getLeaderboardId(leaderboard).c_str(), llValue);
  LeaderboardsClientInterface::submitScore(getLeaderboardId(leaderboard).c_str(), llValue);
  LOGV("After update leaderboard value");
}

void CConnectionInterface::updateAchievementsProgress(SocialGaming::EAchievements achievement,
						      float fPercentComplete) {
  LOGV("Before update achievements progress");
  AchievementsClientInterface::updateProgress(getAchievementId(achievement).c_str(),
					      fPercentComplete,
					      this);
  LOGV("After update achievements progress");
}
void CConnectionInterface::showSocalGamingOverlay() {
  LOGV("Before show achievements overlay");
  AchievementsClientInterface::showAchievementsOverlay();
  //GameCircleClientInterface::showGameCircle();
  LOGV("After");
}

void CConnectionInterface::showSignInPage() {
  GameCircleClientInterface::showSignInPage(this);
}

void CConnectionInterface::onShowSignInPageCb(AmazonGames::ErrorCode errorCode,
					      int developerTag) {
  
}

void CConnectionInterface::onUpdateProgressCb(
					      ErrorCode errorCode,
					      const UpdateProgressResponse* responseStruct,
					      int developerTag) {
  if (errorCode == NO_ERROR) {
    if (responseStruct->isNewlyUnlocked) {
      LOGI("Newly unlocked Achievement");
      updateLeaderboard(GLOAL_RANKING, REASON_ACHIEVEMENT_UNLOCKED);
    }
  }
  else {
    LOGW("Error in onUpdateProgressCb %d", errorCode);
  }
}

void CConnectionInterface::onGetAchievementsCb(
					       AmazonGames::ErrorCode errorCode,
					       const AmazonGames::AchievementsData* responseStruct,
					       int developerTag) {
  if (errorCode == AmazonGames::NO_ERROR) {
    for (int i = 0; i < responseStruct->numAchievements; i++) {
      const AchievementData data = responseStruct->achievements[i];
      SAchievementData &achievment = m_Achievements[getAchievementId(data.id)];
      achievment.id = getAchievementId(data.id);
      achievment.unlocked = data.isUnlocked;
      achievment.pointValue = data.pointValue;
      achievment.progress = data.progress;
    }
    onLeaderboadReasonUpdated(REASON_ACHIEVEMENT_UNLOCKED);
  }
  else {
    LOGW("error onGetAchievementsCb (%d)", errorCode);
  }
}

std::string CConnectionInterface::getAchievementId(SocialGaming::EAchievements achievement) const {
  return GAME_CIRCLE_ACHIEVEMENTS_MAP[achievement];
}
SocialGaming::EAchievements CConnectionInterface::getAchievementId(const std::string &name) const {
  for (int i = 0; i < SocialGaming::ACHIEVEMENT_COUNT; i++) {
    if (GAME_CIRCLE_ACHIEVEMENTS_MAP[i] == name) {
      return static_cast<SocialGaming::EAchievements>(i);
    }
  }
  LOGW("Achievement id %s not defined", name.c_str());
  return SocialGaming::DEBUG_ACHIEVEMENT;
}

std::string CConnectionInterface::getLeaderboardId(SocialGaming::ELeaderboards leaderboard) const {
  return GAME_CIRCLE_LEADERBOARDS_MAP[leaderboard];
}
SocialGaming::ELeaderboards CConnectionInterface::getLeaderboardId(const std::string &name) const {
  for (int i = 0; i < SocialGaming::LEADERBOARDS_COUNT; i++) {
    if (GAME_CIRCLE_LEADERBOARDS_MAP[i] == name) {
      return static_cast<SocialGaming::ELeaderboards>(i);
    }
  }
  LOGW("Achievement id %s not defined", name.c_str());
  return SocialGaming::LEADERBOARDS_COUNT;
}

#endif // MENCUS_USE_AMAZON_GAME_CIRCLE
