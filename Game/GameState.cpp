#include "GameState.hpp"
#include "MainMenu.hpp"
#include "Screenplay.hpp"
#include "GUIGameOver.hpp"

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
  m_pSaveState(NULL) {
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
void CGameState::changeGameState(EGameStates eNewGameState) {
  m_eNextGameState = eNewGameState;
}
void CGameState::changeGameStateImpl() {
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
  m_eCurrentGameState = m_eNextGameState;
  switch (m_eCurrentGameState) {
  case GS_GAME:
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
