#include "GameState.hpp"
#include "MainMenu.hpp"
#include "Screenplay.hpp"
#include "GUIGameOver.hpp"
#include "AdDisplayManager.hpp"

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
  m_bForce(true) {
}
CGameState::~CGameState() {
  if (m_pScreenplay) {
    delete m_pScreenplay;
    m_pScreenplay = NULL;
  }
}
void CGameState::init() {
  m_pMainMenu = CMainMenu::getSingletonPtr();
}
void CGameState::changeGameState(EGameStates eNewGameState, bool bNow, bool bForce) {
  m_eNextGameState = eNewGameState;
  if (bNow) {
    changeGameStateImpl();
  }
}
void CGameState::changeGameStateImpl() {
  if (!m_bForce && m_eCurrentGameState == m_eNextGameState) {
    return;
  }
  auto ePreviousGameState = m_eCurrentGameState;
  switch (m_eCurrentGameState) {
  case GS_GAME:
    delete m_pScreenplay;
    m_pScreenplay = NULL;
    break;
  case GS_MAIN_MENU:
    m_pMainMenu->hide();
    break;
  case GS_GAME_OVER:
    CGUIGameOver::getSingleton().hide();
    break;
  default:
    break;
  }
  try {
    m_eCurrentGameState = m_eNextGameState;
    switch (m_eCurrentGameState) {
    case GS_GAME:
      CAdDisplayManager::showAdPopup();
      m_pScreenplay = new CScreenplay();
      break;
    case GS_MAIN_MENU:
      m_pMainMenu->show();
      m_pMainMenu->changeState(CMainMenu::MMS_START);
      break;
    case GS_GAME_OVER:
      CGUIGameOver::getSingleton().show();
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
  default:
    break;
  }
}
