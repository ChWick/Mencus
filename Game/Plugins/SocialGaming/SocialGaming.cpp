#include "SocialGaming.hpp"
#include "GlobalBuildDefines.hpp"
#include "Log.hpp"
#include "Sleep.hpp"
#include "Game.hpp"
#include "HUDMessageBox.hpp"

#include <OgrePlatform.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android/Android.hpp"
#endif

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include "GameCircle.hpp"
#else
#include "SocialGamingConnectionInterface.hpp"
#endif

using namespace SocialGaming;

template<> CSocialGaming  *Ogre::Singleton<CSocialGaming>::msSingleton = 0;

CSocialGaming::CSocialGaming() 
  : m_eConnectionStatus(DISCONNECTED),
    m_fConnectionCheckTimer(0) {
#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
  m_pConnection = new GameCircle::CConnectionInterface();
#else
  // dummy to avoid nullptr checks
  m_pConnection = new CSocialGamingConnectionInterface();
#endif
}

void CSocialGaming::init() {
  if (m_pConnection->init()) {
    m_eConnectionStatus = CONNECTED;
    m_pConnection->changeSignedIn(true);
  }
  else {
    m_eConnectionStatus = CONNECTING;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    OgreAndroidBridge::callJavaVoid("startPlugins");
#else
    LOGW("Restart not implemented");
#endif
  }
}
void CSocialGaming::update(float tpf) {
  if (m_eConnectionStatus == CONNECTING) {
    m_fConnectionCheckTimer -= tpf;
    if (m_fConnectionCheckTimer <= 0) {
      LOGV("Init connection check");
      m_fConnectionCheckTimer = 3.0f;
      bool success = m_pConnection->init();
      if (success) {
	m_eConnectionStatus = CONNECTED;
	LOGI("Connected to SocialGaming");
  
	std::vector<std::string> pages;
	pages.push_back("Connected");
	new CHUDMessageBox("blank", m_pConnection->getName(), pages);

	m_pConnection->changeSignedIn(true);
      }
    }
  }
}
void CSocialGaming::update(const SStatistics &stats) {
  if (stats.eMissionState == MS_ACCOMPLISHED) {
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
      updateAchievementsProgress(DEBUG_ACHIEVEMENT, 100);
    }
  }
}

void CSocialGaming::updateAchievementsProgress(EAchievements achievement, float fPercentComplete) {  
  m_pConnection->updateAchievementsProgress(achievement, fPercentComplete);
}
