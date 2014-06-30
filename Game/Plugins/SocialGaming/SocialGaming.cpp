#include "SocialGaming.hpp"
#include "GlobalBuildDefines.hpp"
#include "Log.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include "GameCircle.hpp"
#else
#include "SocialGamingConnectionInterface.hpp"
#endif

using namespace SocialGaming;

template<> CSocialGaming  *Ogre::Singleton<CSocialGaming>::msSingleton = 0;

CSocialGaming::CSocialGaming() {
#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
  m_pConnection = new GameCircle::CConnectionInterface();
#else
  // dummy to avoid nullptr checks
  m_pConnection = new CSocialGamingConnectionInterface();
#endif
}

void CSocialGaming::update(const SStatistics &stats) {
  if (stats.eMissionState == MS_ACCOMPLISHED) {
    LOGV("Level finished in social gaming: %s", stats.sLevelFileName.c_str());
    if (stats.sLevelFileName == "Tutorial_1") {
      updateAchievementsProgress(DEBUG_ACHIEVEMENT, 25);
    }
    else if (stats.sLevelFileName == "Tutorial_2") {
      updateAchievementsProgress(DEBUG_ACHIEVEMENT, 50);
    }
    else if (stats.sLevelFileName == "Tutorial_3") {
      updateAchievementsProgress(DEBUG_ACHIEVEMENT, 75);
    }
    else if (stats.sLevelFileName == "Tutorial_4") {
      updateAchievementsProgress(DEBUG_ACHIEVEMENT, 100);
    }
  }
}

void CSocialGaming::updateAchievementsProgress(EAchievements achievement, float fPercentComplete) {
  m_afAchievements[achievement] = fPercentComplete;
  m_pConnection->updateAchievementsProgress(achievement, fPercentComplete);
}
