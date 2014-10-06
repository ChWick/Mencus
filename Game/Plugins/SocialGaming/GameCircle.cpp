#include "GameCircle.hpp"
#include "Sleep.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include "Log.hpp"

using namespace AmazonGames;

using namespace GameCircle;


CConnectionInterface::CConnectionInterface() 
  : CSocialGamingConnectionInterface("Amazon GameCircle",
				     "AmazonGameCircle") {
}

bool CConnectionInterface::init() {
  PlayerClientInterface::setSignedInStateChangedListener(this);
  return PlayerClientInterface::isSignedIn();
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


void CConnectionInterface::onUpdateProgressCb(
					      ErrorCode errorCode,
					      const UpdateProgressResponse* responseStruct,
					      int developerTag) {
  LOGV("onUpdateProgressCb %d", errorCode);
}

std::string CConnectionInterface::getAchievementId(SocialGaming::EAchievements achievement) {
  switch (achievement) {
  case SocialGaming::DEBUG_ACHIEVEMENT:
    return "boot_1";
  case SocialGaming::BOOTS_1:
    return "boot_1";
  case SocialGaming::BOOTS_2:
    return "boot_2";
  case SocialGaming::BOOTS_3:
    return "boot_3";
  default:
    LOGW("Achievement not defined in %s", __FILE__);
  }
}


#endif // MENCUS_USE_AMAZON_GAME_CIRCLE
