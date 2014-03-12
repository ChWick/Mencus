#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <OgreSingleton.h>

class CMainMenu;
class CScreenplay;
class CSaveState;

class CGameState : public Ogre::Singleton<CGameState> {
public:
  enum EGameStates {
    GS_MAIN_MENU,
    GS_GAME,
    GS_GAME_OVER,
    GS_CREDITS,

    GS_COUNT
  };
private:
  EGameStates m_eCurrentGameState;
  EGameStates m_eNextGameState;
  CMainMenu *m_pMainMenu;
  CScreenplay *m_pScreenplay;
  const CSaveState *m_pSaveState;
  bool m_bForce;

public:
  static CGameState &getSingleton();
  static CGameState *getSingletonPtr();

  CGameState();
  ~CGameState();

  void init();

  void update(Ogre::Real tpf);

  void changeGameState(EGameStates eNewGameState, bool bNow = false, bool bForce = true);
  void setSaveState(const CSaveState *pState) {m_pSaveState = pState;}
  const CSaveState *getSaveState() const {return m_pSaveState;}
  EGameStates getCurrentGameState() {return m_eCurrentGameState;}
  CScreenplay *getScreenplay() {return m_pScreenplay;}

private:
  void changeGameStateImpl();
};

#endif
