/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

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
#include "LevelList.hpp"
#include "MainMenuStates.hpp"

class CMapInfo;
class CMapPack;
class CSaveState;

extern const float BUTTON_MIN_ALPHA;
extern const float BUTTON_ALPHA_CHANGE_PER_SEC;

class CMainMenu :
  public Ogre::Singleton<CMainMenu>,
  public CInputListener,
  public CPauseCaller {
public:
  static const int NUM_SLOTS = 4;
  
  enum EStartSlots {
    START_START_GAME  = 0,
    START_OPTIONS     = 1,
    START_CREDITS     = 2,
    START_EXIT        = NUM_SLOTS - 1,
  };
  enum EGameSlots {
#if ENABLE_CAMPAIGN == 0
    GAME_USER_GAME    = 0,
#if ENABLE_MAP_EDITOR
    GAME_MAP_EDITOR   = 1,
#endif
#else
    GAME_NEW_GAME     = 0,
    GAME_USER_GAME    = 1,
    GAME_LOAD_GAME    = 2,
#endif
    GAME_BACK         = NUM_SLOTS - 1,
  };
  enum EMapEditorSlots {
    MAP_EDITOR_NEW_MAP    = 0,
    MAP_EDITOR_LOAD_MAP   = 1,

    MAP_EDITOR_BACK       = NUM_SLOTS - 1,
  };
  enum EMapEditorLoadSlots {
    MAP_EDITOR_LOAD_BACK  = NUM_SLOTS - 1
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
  // level selection windows
  CLevelList m_LevelList;
  CEGUI::Window *m_pLevelSelection;
  unsigned int m_uiSelectedLevelID;
  SLevelInfo *m_pLevelInfo;

  CEGUI::Window *m_pMMRoot;
  CEGUI::Window *m_pButtonContainer;
  CEGUI::Window *m_pOptionPages[OPTIONS_BACK];
  std::vector<CEGUI::Window *> m_vSlots;
  MainMenu::EState m_iTargetState[MainMenu::MMS_COUNT][NUM_SLOTS];
  CEGUI::String m_sButtonLabels[MainMenu::MMS_COUNT][NUM_SLOTS];
  CEGUI::Window *m_pSelectButton;
  MainMenu::EState m_eCurrentState;
  int m_iSelectedSlot;
  CEGUI::Listbox *m_pSaveStatesWindow;
  CEGUI::Window *m_pSaveStatePreviewWindow;
  CEGUI::Window *m_pMapInfoContainer;
  CEGUI::Window *m_pMapInfoWindow;
  bool m_bSaveListSelected;
  int m_iSelectedLoadState;
  const CSaveState *m_pStateToLoad;
  std::vector<Ogre::String> m_vUserFiles;
  std::shared_ptr<CMapPack> m_pMapPack;
public:
  static CMainMenu &getSingleton();
  static CMainMenu *getSingletonPtr();

  CMainMenu(CEGUI::Window *pGUIRoot);
  ~CMainMenu();

  void createResources();

  void update(Ogre::Real tpf);

  void changeState(MainMenu::EState eState);

  virtual bool keyPressed(const OIS::KeyEvent &arg);

  void show();
  void hide();

  void windowSizeChanged(const CEGUI::Sizef &vSize);
  void resizeGUI(Ogre::Real fScaling);
private:
  // button setup
  void setupButton(MainMenu::EState eState, unsigned int uiButtonID, const CEGUI::String &sLabel, MainMenu::EState iTargetID);

  // level selection
  void updateLevelsSelection();
  void selectLevel(unsigned int id);
  bool onLevelButtonClicked(const CEGUI::EventArgs &);
  bool onChickenPressed(const CEGUI::EventArgs&);

  void selectedSaveStateChanged();
  void activateLoadState();

  bool buttonSelected(const CEGUI::EventArgs&);
  bool buttonClicked(const CEGUI::EventArgs&);
  bool saveStateListEntered(const CEGUI::EventArgs&);
  bool saveStateListLeaved(const CEGUI::EventArgs&);
  bool saveStateListMouseMoved(const CEGUI::EventArgs&);
  bool onSelectionChanged(const CEGUI::EventArgs&);

  bool onSelectButtonClicked(const CEGUI::EventArgs&);

};

#endif // MAIN_MENU_HPP
