#include "GameCircle.hpp"
#include "Sleep.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include "Log.hpp"

using namespace AmazonGames;

using namespace GameCircle;


CConnectionInterface::CConnectionInterface() {
  PlayerClientInterface::setSignedInStateChangedListener(this);
  changeSignedIn(PlayerClientInterface::isSignedIn());
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
  default:
    LOGW("Achievement not defined in %s", __FILE__);
  }
}


#endif // MENCUS_USE_AMAZON_GAME_CIRCLE
