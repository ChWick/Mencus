#ifndef __GAME_CIRCLE_HPP__
#define __GAME_CIRCLE_HPP__

#include "GlobalBuildDefines.hpp"
#include "SocialGamingConnectionInterface.hpp"
#include <string>

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1

#include "includes/AchievementsClientInterface.h"
#include "includes/PlayerClientInterface.h"
#include "includes/GameCircleClientInterface.h"

namespace GameCircle {

  class CConnectionInterface
    : public SocialGaming::CSocialGamingConnectionInterface,
      public AmazonGames::ISignedInStateChangedListener,
      public AmazonGames::IUpdateProgressCb {
  public:
    CConnectionInterface();
    
    void loadAchievements() {}
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
  private:
    std::string getAchievementId(SocialGaming::EAchievements achievement);
  };  
};

#endif // MENCUS_USE_AMAZON_GAME_CIRCLE

#endif
