#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <OgreSingleton.h>
#include <memory>
#include "MainMenuStates.hpp"


class CMainMenu;
class CScreenplay;
class CSaveState;
class CMapInfo;
class CGUICredits;

class CGameState : public Ogre::Singleton<CGameState> {
public:
  enum EGameStates {
    GS_MAIN_MENU,
    GS_GAME,
    GS_GAME_OVER,
    GS_STATISTICS,
    GS_CREDITS,
    GS_AD,

    GS_COUNT
  };
private:
  EGameStates m_eCurrentGameState;
  EGameStates m_eNextGameState;
  CMainMenu *m_pMainMenu;
  MainMenu::EState m_eMainMenuState;
  CScreenplay *m_pScreenplay;
  const CSaveState *m_pSaveState;
  std::shared_ptr<const CMapInfo> m_pMapInfo;
  bool m_bForce;
  bool m_bAdShown;

  CGUICredits *m_pCredits;	//!< Holds the credits, if they are shown
public:
  static CGameState &getSingleton();
  static CGameState *getSingletonPtr();

  CGameState();
  ~CGameState();

  void init();

  void update(Ogre::Real tpf);

  void changeGameState(EGameStates eNewGameState, bool bNow = false, bool bForce = true);
  void changeGameState(EGameStates eNewGameState, const CSaveState *pState);
  void changeGameState(EGameStates eNewGameState, std::shared_ptr<const CMapInfo> pInfo);
  void changeGameState(EGameStates eNewGameState, MainMenu::EState eMainMenuState);

  const CSaveState *getSaveState() const {return m_pSaveState;}
  std::shared_ptr<const CMapInfo> getMapInfo() const {return m_pMapInfo;}
  void setMapInfo(std::shared_ptr<const CMapInfo> ptr) {m_pMapInfo = ptr;}
  EGameStates getCurrentGameState() {return m_eCurrentGameState;}
  CScreenplay *getScreenplay() {return m_pScreenplay;}
  void setAdShown(bool bShown) {m_bAdShown = bShown;}

private:
  void changeGameStateImpl();
};

#endif
