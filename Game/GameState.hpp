#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <OgreSingleton.h>

class CMainMenu;
class CScreenplay;

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
  CMainMenu *m_pMainMenu;
  CScreenplay *m_pScreenplay;

public:
  static CGameState &getSingleton();
  static CGameState *getSingletonPtr();

  CGameState();
  ~CGameState();

  void update(Ogre::Real tpf);

  void changeGameState(EGameStates eNewGameState);
};

#endif