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
