#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <OgreSingleton.h>
#include <memory>

class CMainMenu;
class CScreenplay;
class CSaveState;
class CMapInfo;

class CGameState : public Ogre::Singleton<CGameState> {
public:
  enum EGameStates {
    GS_MAIN_MENU,
    GS_GAME,
    GS_GAME_OVER,
    GS_STATISTICS,
    GS_CREDITS,

    GS_COUNT
  };
private:
  EGameStates m_eCurrentGameState;
  EGameStates m_eNextGameState;
  CMainMenu *m_pMainMenu;
  CScreenplay *m_pScreenplay;
  const CSaveState *m_pSaveState;
  std::shared_ptr<const CMapInfo> m_pMapInfo;
  bool m_bForce;

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
  const CSaveState *getSaveState() const {return m_pSaveState;}
  std::shared_ptr<const CMapInfo> getMapInfo() const {return m_pMapInfo;}
  EGameStates getCurrentGameState() {return m_eCurrentGameState;}
  CScreenplay *getScreenplay() {return m_pScreenplay;}

private:
  void changeGameStateImpl();
};

#endif
