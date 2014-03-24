#include "GameState.hpp"
#include "MainMenu.hpp"
#include "Screenplay.hpp"
#include "GUIGameOver.hpp"
#include "AdDisplayManager.hpp"
#include "MapInfo.hpp"
#include "GUIStatistics.hpp"
#include <unistd.h>

using namespace std;

template<> CGameState *Ogre::Singleton<CGameState>::msSingleton = 0;

CGameState &CGameState::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}
CGameState *CGameState::getSingletonPtr() {
  return msSingleton;
}

CGameState::CGameState()
: m_eCurrentGameState(GS_COUNT),
  m_eNextGameState(GS_COUNT),
  m_pMainMenu(NULL),
  m_pScreenplay(NULL),
  m_pSaveState(NULL),
  m_bForce(true),
  m_bAdShown(false) {
}
CGameState::~CGameState() {
  if (m_pScreenplay) {
    delete m_pScreenplay;
    m_pScreenplay = NULL;
  }
}
void CGameState::init() {
  m_pMainMenu = CMainMenu::getSingletonPtr();
  m_bAdShown = false;
}
void CGameState::changeGameState(EGameStates eNewGameState, bool bNow, bool bForce) {
  m_eNextGameState = eNewGameState;
  //m_pMapInfo.reset();
  //m_pSaveState = NULL;
  if (bNow) {
    changeGameStateImpl();
  }
}
void CGameState::changeGameState(EGameStates eNewGameState, const CSaveState *pState) {
  m_eNextGameState = eNewGameState;
  m_pMapInfo.reset();
  m_pSaveState = pState;
}
void CGameState::changeGameState(EGameStates eNewGameState, std::shared_ptr<const CMapInfo> pInfo) {
  m_eNextGameState = eNewGameState;
  m_pMapInfo = pInfo;
  m_pSaveState = NULL;
}
void CGameState::changeGameStateImpl() {
  if (!m_bForce && m_eCurrentGameState == m_eNextGameState) {
    return;
  }
  auto ePreviousGameState = m_eCurrentGameState;
  if (m_eNextGameState == GS_GAME && !m_bAdShown) {
    m_eNextGameState = GS_AD;
  }
  switch (m_eCurrentGameState) {
  case GS_GAME:
    m_bAdShown = false;
    delete m_pScreenplay;
    m_pScreenplay = NULL;
    break;
  case GS_AD:
    m_bAdShown = false;
    break;
  case GS_MAIN_MENU:
    m_pMainMenu->hide();
    break;
  case GS_GAME_OVER:
    CGUIGameOver::getSingleton().hide();
    break;
  case GS_STATISTICS:
    CGUIStatistics::getSingleton().hide();
    break;
  default:
    break;
  }
  try {
    m_eCurrentGameState = m_eNextGameState;
    switch (m_eCurrentGameState) {
    case GS_GAME:
      m_pScreenplay = new CScreenplay();
      if (m_pMapInfo) {
	m_pScreenplay->loadSingleMap(m_pMapInfo);
      }
      break;
    case GS_MAIN_MENU:
      m_pMainMenu->show();
      m_pMainMenu->changeState(CMainMenu::MMS_START);
      break;
    case GS_GAME_OVER:
      CGUIGameOver::getSingleton().show();
      break;
    case GS_STATISTICS:
      CGUIStatistics::getSingleton().show();
      break;
    case GS_AD:
      CAdDisplayManager::showAdPopup();
      break;
    default:
      break;
    }
  }
  catch (const Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage("Exception while changing the game state");
    Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
    m_eNextGameState = ePreviousGameState;
    changeGameStateImpl();
  }
}
void CGameState::update(Ogre::Real tpf) {
  if (m_eNextGameState != m_eCurrentGameState) {
    changeGameStateImpl();
  }

  switch (m_eCurrentGameState) {
  case GS_GAME:
    m_pScreenplay->update(tpf);
    break;
  case GS_GAME_OVER:
    CGUIGameOver::getSingleton().update(tpf);
    break;
  case GS_STATISTICS:
    CGUIStatistics::getSingleton().update(tpf);
    break;
  case GS_AD:
    if (CAdDisplayManager::adPopupClosed()) {
      m_bAdShown = true;
      Ogre::LogManager::getSingleton().logMessage("Add finished");
      m_eNextGameState = GS_GAME;
      changeGameStateImpl();
    }
    else {  
      Ogre::LogManager::getSingleton().logMessage("Waiting for add finished");
      usleep(1000 * 500); // 0.5 secs  default:
    }
    break;
  }
}
