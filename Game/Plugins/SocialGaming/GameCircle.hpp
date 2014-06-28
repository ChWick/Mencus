#ifndef __GAME_CIRCLE_HPP__
#define __GAME_CIRCLE_HPP__

#include "GlobalBuildDefines.hpp"
#include "SocialGamingConnectionInterface.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1

#include "includes/AchievementsClientInterface.h"

namespace GameCircle {

class CConnectionInterface : public SocialGaming::CSocialGamingConnectionInterface {
public:
void loadAchievements() {}
};

  class GetAchCbImpl : public AmazonGames::IGetAchievementCb {
public:
GetAchCbImpl();
~GetAchCbImpl();
 
void onGetAchievementCb(
AmazonGames::ErrorCode errorCode,
  const AmazonGames::AchievementData* responseStruct,
  int developerTag);
};



};

#endif // MENCUS_USE_AMAZON_GAME_CIRCLE

#endif
