#include "AdDisplayManager.hpp"
#include "Game.hpp"
#include <OgreLogManager.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android/Android.hpp"
#endif

int CAdDisplayManager::m_iWaitingLoops(10);
int CAdDisplayManager::m_iWaitedLoops(0);

void CAdDisplayManager::showAdPopup() {
  m_iWaitedLoops = 0;
  CGame::getSingleton().showLoadingBar(0, 1);
  CGame::getSingleton().getTrayMgr()->resourceGroupLoadStarted("Please wait", m_iWaitingLoops);
  CGame::getSingleton().getTrayMgr()->worldGeometryStageStarted("Please wait");
#ifdef ADDS_ANDROID
  m_iWaitingLoops = 14; // 14 * 0.5 secs max waiting time for the add
  OgreAndroidBridge::showAdPopup();
#else
  m_iWaitingLoops = 0; // set to value > 0 for testing
#endif
}
bool CAdDisplayManager::adPopupClosed() {
  if (isAdPopupClosed()) {
    CGame::getSingleton().hideLoadingBar();
    return true;
  }
  return false;
}
bool CAdDisplayManager::isAdPopupClosed() {
  // to increase loading bar
  CGame::getSingleton().getTrayMgr()->resourceLoadEnded();
  m_iWaitedLoops += 1;
#ifdef ADDS_ANDROID
  if (m_iWaitedLoops > m_iWaitingLoops) {
    // force close of ad, not displaying
    OgreAndroidBridge::closeAd();
    return true;
  }
  return OgreAndroidBridge::adPopupClosed();
#else
  return m_iWaitedLoops > m_iWaitingLoops;
#endif
}
