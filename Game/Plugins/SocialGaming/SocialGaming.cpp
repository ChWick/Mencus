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

CSocialGaming::CSocialGaming() {
#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
  m_pConnection = new GameCircle::CConnectionInterface();
#else
  // dummy to avoid nullptr checks
  m_pConnection = new CSocialGamingConnectionInterface();
#endif
}

void CSocialGaming::init() {
  bool success = false;
  if (m_pConnection->init()) {
    success = true;
  }
  else {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    OgreAndroidBridge::callJavaVoid("startPlugins");
#else
    LOGW("Restart not implemented");
#endif
    
    
    
    int iLoop = 0;
    int iMaxLoops = 60;
    
    std::string sLabel = "Connecting to " + m_pConnection->getName();
    
    CGame::getSingleton().showLoadingBar(0, 1);
    CGame::getSingleton().getTrayMgr()->resourceGroupLoadStarted(sLabel, iMaxLoops);
    CGame::getSingleton().getTrayMgr()->worldGeometryStageStarted(sLabel);
    
    while (!(success = m_pConnection->init())) {
      LOGV("Waiting for login: %d", iLoop);
      usleep(1000 * 500);
      // to increase loading bar
      CGame::getSingleton().getTrayMgr()->resourceLoadEnded();
      iLoop++;
      if (iLoop > iMaxLoops) {
        success = false;
        break;
      }
    }
    
    
    CGame::getSingleton().hideLoadingBar();
  }
    
  m_pConnection->changeSignedIn(success);
    
  std::vector<std::string> pages;
  if (success) {
    pages.push_back("Connected!");
  }
  else {
    pages.push_back("Connection timed out.");
  }
  
  new CHUDMessageBox("blank", m_pConnection->getName(), pages);
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
