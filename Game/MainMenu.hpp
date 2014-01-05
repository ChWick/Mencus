#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <CEGUI/CEGUI.h>
#include <OgreSingleton.h>
#include <vector>
#include "InputListener.hpp"

class CMainMenu : public Ogre::Singleton<CMainMenu>, public CInputListener {
public:
  static const int NUM_SLOTS = 4;
  enum EMainMenuState {
    MMS_STATE_NONE = -1,

    MMS_START = 0,
    MMS_GAME,
    MMS_OPTIONS,

    MMS_COUNT,

    MMS_EXIT,
    MMS_NEW_GAME,
    MMS_LOAD_GAME
  };
  enum EStartSlots {
    START_START_GAME  = 0,
    START_OPTIONS     = 1,
    START_EXIT        = NUM_SLOTS - 1,
  };
  enum EGameSlots {
    GAME_NEW_GAME     = 0,
    GAME_LOAD_GAME    = 1,
    GAME_BACK         = NUM_SLOTS - 1,
  };
private:
  CEGUI::Window *m_pMMRoot;
  std::vector<CEGUI::Window *> m_vSlots;
  EMainMenuState m_iTargetState[MMS_COUNT][NUM_SLOTS];
  CEGUI::String m_sButtonLabels[MMS_COUNT][NUM_SLOTS];
  EMainMenuState m_eCurrentState;
  int m_iSelectedSlot;
public:
  static CMainMenu &getSingleton();
  static CMainMenu *getSingletonPtr();

  CMainMenu(CEGUI::Window *pGUIRoot);
  ~CMainMenu();

  void update(Ogre::Real tpf);

  void changeState(EMainMenuState eState);

  virtual bool keyPressed(const OIS::KeyEvent &arg);

  void show() { m_pMMRoot->setVisible(true); setInputListenerEnabled(true); }
  void hide() { m_pMMRoot->setVisible(false); setInputListenerEnabled(false); }
};

#endif // MAIN_MENU_HPP
