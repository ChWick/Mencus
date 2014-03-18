#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <CEGUI/CEGUI.h>
#include <OgreSingleton.h>
#include <vector>
#include <list>
#include "InputListener.hpp"
#include "PauseCaller.hpp"
#include <memory>
#include "GlobalBuildDefines.hpp"

class CMapInfo;
class CSaveState;

extern const float BUTTON_MIN_ALPHA;
extern const float BUTTON_ALPHA_CHANGE_PER_SEC;

class CMainMenu : public Ogre::Singleton<CMainMenu>, public CInputListener, public CPauseCaller {
public:
  static const int NUM_SLOTS = 4;
  enum EMainMenuState {
    MMS_STATE_NONE = -1,

    MMS_START = 0,
    MMS_GAME,
    MMS_LOAD_GAME,
    MMS_OPTIONS,
    MMS_OPTIONS_VIDEO,
    MMS_OPTIONS_INPUT,
    MMS_GAME_ESCAPE,
    MMS_USER_GAME,

    MMS_COUNT,

    MMS_RESULT_BACK_TO_GAME,
    MMS_RESULT_EXIT,
    MMS_RESULT_NEW_GAME,
    MMS_RESULT_LOAD_GAME,
  };
  enum EStartSlots {
    START_START_GAME  = 0,
    START_OPTIONS     = 1,
    START_EXIT        = NUM_SLOTS - 1,
  };
  enum EGameSlots {
#ifdef DISABLE_CAMPAIGN
    GAME_USER_GAME    = 0,
#else
    GAME_NEW_GAME     = 0,
    GAME_USER_GAME    = 1,
    GAME_LOAD_GAME    = 2,
#endif
    GAME_BACK         = NUM_SLOTS - 1,
  };
  enum ELoadGameSlots {
    LOAD_GAME_BACK    = NUM_SLOTS - 1,
  };
  enum EOptionsSlots {
    OPTIONS_VIDEO   = 0,
    OPTIONS_INPUT   = 1,
    OPTIONS_BACK    = NUM_SLOTS - 1,
  };
  enum EOptionsVideoSlots {
    OPTIONS_VIDEO_BACK = NUM_SLOTS - 1,
  };
  enum EOptionsInputSlots {
    OPTIONS_INPUT_BACK = NUM_SLOTS - 1,
  };
  enum EGamesEscapeSlots {
    GAMES_ESCAPE_BACK_TO_GAME = 0,
    GAMES_ESCAPE_OPTIONS      = 1,
    GAMES_ESCAPE_EXIT_GAME    = NUM_SLOTS - 1,
  };
  enum EUserGameSlots {
    USER_GAME_BACK    = NUM_SLOTS - 1,
  };
private:
  CEGUI::Window *m_pMMRoot;
  CEGUI::Window *m_pButtonContainer;
  CEGUI::Window *m_pOptionPages[OPTIONS_BACK];
  std::vector<CEGUI::Window *> m_vSlots;
  EMainMenuState m_iTargetState[MMS_COUNT][NUM_SLOTS];
  CEGUI::String m_sButtonLabels[MMS_COUNT][NUM_SLOTS];
  CEGUI::Window *m_pSelectButton;
  EMainMenuState m_eCurrentState;
  int m_iSelectedSlot;
  CEGUI::Listbox *m_pSaveStatesWindow;
  CEGUI::Window *m_pSaveStatePreviewWindow;
  CEGUI::Window *m_pMapInfoWindow;
  bool m_bSaveListSelected;
  int m_iSelectedLoadState;
  const CSaveState *m_pStateToLoad;
  std::vector<Ogre::String> m_vUserFiles;
  std::shared_ptr<CMapInfo> m_pMapInfo;
public:
  static CMainMenu &getSingleton();
  static CMainMenu *getSingletonPtr();

  CMainMenu(CEGUI::Window *pGUIRoot);
  ~CMainMenu();

  void createResources();

  void update(Ogre::Real tpf);

  void changeState(EMainMenuState eState);

  virtual bool keyPressed(const OIS::KeyEvent &arg);

  void show() { m_pMMRoot->setVisible(true); setInputListenerEnabled(true); }
  void hide() { m_pMMRoot->setVisible(false); setInputListenerEnabled(false); }

  void windowSizeChanged(const CEGUI::Sizef &vSize);
private:
  void selectedSaveStateChanged();
  void activateLoadState();

  bool buttonSelected(const CEGUI::EventArgs&);
  bool buttonClicked(const CEGUI::EventArgs&);
  bool saveStateListEntered(const CEGUI::EventArgs&);
  bool saveStateListLeaved(const CEGUI::EventArgs&);
  bool saveStateListMouseMoved(const CEGUI::EventArgs&);
  bool onSelectionChanged(const CEGUI::EventArgs&);

  bool buttonSizeSliderValueChanged(const CEGUI::EventArgs&);
  bool menuSizeSliderValueChanged(const CEGUI::EventArgs&);

  bool onSelectButtonClicked(const CEGUI::EventArgs&);

  void resizeGUI(Ogre::Real fScaling);
};

#endif // MAIN_MENU_HPP
