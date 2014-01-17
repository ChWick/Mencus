#include "GameState.hpp"
#include "MainMenu.hpp"
#include "Screenplay.hpp"

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
  m_pMainMenu(CMainMenu::getSingletonPtr()),
  m_pScreenplay(NULL),
  m_pSaveState(NULL) {
  changeGameState(GS_MAIN_MENU);
}
CGameState::~CGameState() {
  if (m_pScreenplay) {
    delete m_pScreenplay;
    m_pScreenplay = NULL;
  }
}
void CGameState::changeGameState(EGameStates eNewGameState) {
  switch (m_eCurrentGameState) {
  case GS_GAME:
    delete m_pScreenplay;
    m_pScreenplay = NULL;
    break;
  case GS_MAIN_MENU:
    m_pMainMenu->hide();
    break;
  default:
    break;
  }
  m_eCurrentGameState = eNewGameState;
  switch (m_eCurrentGameState) {
  case GS_GAME:
    m_pScreenplay = new CScreenplay();
    break;
  case GS_MAIN_MENU:
    m_pMainMenu->show();
    m_pMainMenu->changeState(CMainMenu::MMS_START);
    break;
  default:
    break;
  }
}
void CGameState::update(Ogre::Real tpf) {
  switch (m_eCurrentGameState) {
  case GS_GAME:
    m_pScreenplay->update(tpf);
    break;
  default:
    break;
  }
}
