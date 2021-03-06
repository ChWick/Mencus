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

#ifndef __GAME_CIRCLE_HPP__
#define __GAME_CIRCLE_HPP__

#include "GlobalBuildDefines.hpp"
#include "SocialGamingConnectionInterface.hpp"
#include <string>

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1

#include "includes/AchievementsClientInterface.h"
#include "includes/PlayerClientInterface.h"
#include "includes/GameCircleClientInterface.h"
#include "includes/LeaderboardsClientInterface.h"

namespace GameCircle {

  class CConnectionInterface
    : public SocialGaming::CSocialGamingConnectionInterface,
      public AmazonGames::IShowSignInPageCb,
      public AmazonGames::ISignedInStateChangedListener,
      public AmazonGames::IUpdateProgressCb,
      public AmazonGames::IGetAchievementsCb {
  private:
    static std::string GAME_CIRCLE_ACHIEVEMENTS_MAP[SocialGaming::ACHIEVEMENT_COUNT];
    static std::string GAME_CIRCLE_LEADERBOARDS_MAP[SocialGaming::LEADERBOARDS_COUNT];
  public:
    CConnectionInterface();

    bool init();
    
    void loadAchievements();
    void updateLeaderboardValue(SocialGaming::ELeaderboards leaderboard, long long llValue);
    void updateAchievementsProgress(SocialGaming::EAchievements achievement,
				    float fPercentComplete);
    void showSocalGamingOverlay();
    void showSignInPage();

    // IShowSignInPageCb
    void onShowSignInPageCb(AmazonGames::ErrorCode errorCode,
			    int developerTag);

    // ISignedInStateChangedListener
    void onSignedInStateChanged(bool isSignedIn) {changeSignedIn(isSignedIn);}

    // IUpdateProgressCb
    void onUpdateProgressCb(
			    AmazonGames::ErrorCode errorCode,
			    const AmazonGames::UpdateProgressResponse* responseStruct,
			    int developerTag);

    // IGetAchievementsCb
    void onGetAchievementsCb(
			     AmazonGames::ErrorCode errorCode,
			     const AmazonGames::AchievementsData* responseStruct,
			     int developerTag);
  private:
    std::string getAchievementId(SocialGaming::EAchievements achievement) const;
    SocialGaming::EAchievements getAchievementId(const std::string &name) const;
    std::string getLeaderboardId(SocialGaming::ELeaderboards leaderboard) const;
    SocialGaming::ELeaderboards getLeaderboardId(const std::string &name) const;
  };  
};

#endif // MENCUS_USE_AMAZON_GAME_CIRCLE

#endif
