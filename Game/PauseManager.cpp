#include "PauseManager.hpp"
#include "PauseListener.hpp"
#include "PauseCaller.hpp"

template<> CPauseManager* Ogre::Singleton<CPauseManager>::msSingleton = 0;

CPauseManager* CPauseManager::getSingletonPtr(void)
{
  return msSingleton;
}
CPauseManager& CPauseManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

CPauseManager::CPauseManager()
: m_uiOldPauseFlags(PAUSE_NONE) {

  m_mapPauseListener[PAUSE_PLAYER_INPUT] = &CPauseListener::playerInputPauseChanged;
  m_mapPauseListener[PAUSE_PLAYER_MOVEMENT] = &CPauseListener::playerMovePauseChanged;
  m_mapPauseListener[PAUSE_ENEMY_MOVEMENT] = &CPauseListener::enemyMovePauseChanged;
  m_mapPauseListener[PAUSE_VIDEO] = &CPauseListener::videoPauseChanged;
  m_mapPauseListener[PAUSE_MAP_UPDATE] = &CPauseListener::mapUpdatePauseChanged;
  m_mapPauseListener[PAUSE_MAP_RENDER] = &CPauseListener::mapRenderPauseChanged;
}

void CPauseManager::update() {
  unsigned int uiTotalPause = 0;
  for (auto pCaller : m_lPauseCallers) {
      uiTotalPause |= pCaller->getPause();
  }

  for (auto p : m_mapPauseListener) {
    if ((uiTotalPause & p.first) != (m_uiOldPauseFlags & p.first)) {
      for (auto pListener : m_lPauseListeners) {
        (pListener->*p.second)((uiTotalPause & p.first) > 0);
      }
    }
  }


  m_uiOldPauseFlags = uiTotalPause;
}
