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

#include "MainMenu.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "SaveStateManager.hpp"
#include "MathUtil.hpp"
#include <OgreStringConverter.h>
#include <iostream>
#include "InputDefines.hpp"
#include "GUIManager.hpp"
#include "Settings.hpp"
#include "MapInfo.hpp"
#include "MapEditor.hpp"
#include "XMLResources/Manager.hpp"
#include "Plugins/GUISettings/GUIInputSettings.hpp"
#include "Plugins/GUISettings/GUIVideoSettings.hpp"
#include "Plugins/GUILevelSelect.hpp"

using namespace std;
using namespace CEGUI;
using namespace MainMenu;

template<> CMainMenu *Ogre::Singleton<CMainMenu>::msSingleton = 0;

const float BUTTON_MIN_ALPHA = 0.5f;
const float BUTTON_ALPHA_CHANGE_PER_SEC = 1;

CMainMenu *CMainMenu::getSingletonPtr() {
  return msSingleton;
}
CMainMenu &CMainMenu::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CMainMenu::CMainMenu(CEGUI::Window *pGUIRoot)
  : m_vSlots(NUM_SLOTS),
    m_eCurrentState(MMS_COUNT),
    m_bSaveListSelected(false),
    m_iSelectedLoadState(0),
    m_pStateToLoad(NULL) {
  CInputListenerManager::getSingleton().addInputListener(this);
  ImageManager::getSingleton().loadImageset("main_menu_background.imageset");
  //ImageManager::getSingleton().loadImageset("save_pictures.imageset");

  for (int i = 0; i < MMS_COUNT; i++) {
    for (int j = 0; j < NUM_SLOTS; j++) {
      m_iTargetState[i][j] = MMS_STATE_NONE;
      m_sButtonLabels[i][j] = "Undefined";
    }
  }
  // setup slots for the pages
  m_iTargetState[MMS_START][START_START_GAME] = MMS_GAME;
  m_iTargetState[MMS_START][START_OPTIONS] = MMS_OPTIONS;
  m_iTargetState[MMS_START][START_CREDITS] = MMS_RESULT_CREDITS;
  m_iTargetState[MMS_START][START_EXIT] = MMS_RESULT_EXIT;
  m_sButtonLabels[MMS_START][START_START_GAME] = XMLResources::GLOBAL.getCEGUIString("start_game");
  m_sButtonLabels[MMS_START][START_OPTIONS] = XMLResources::GLOBAL.getCEGUIString("options");
  m_sButtonLabels[MMS_START][START_CREDITS] = XMLResources::GLOBAL.getCEGUIString("credits");
  m_sButtonLabels[MMS_START][START_EXIT] = XMLResources::GLOBAL.getCEGUIString("exit");

  m_iTargetState[MMS_GAME][GAME_USER_GAME] = MMS_USER_GAME;
  m_iTargetState[MMS_GAME][GAME_BACK] = MMS_START;
#if ENABLE_MAP_EDITOR
  m_iTargetState[MMS_GAME][GAME_MAP_EDITOR] = MMS_MAP_EDITOR;
  m_sButtonLabels[MMS_GAME][GAME_MAP_EDITOR] = XMLResources::GLOBAL.getCEGUIString("map_editor");
#endif
#if ENABLE_CAMPAIGN
  m_iTargetState[MMS_GAME][GAME_NEW_GAME] = MMS_RESULT_NEW_GAME;
  m_sButtonLabels[MMS_GAME][GAME_NEW_GAME] = XMLResources::GLOBAL.getCEGUIString("new_game");
  m_iTargetState[MMS_GAME][GAME_LOAD_GAME] = MMS_LOAD_GAME;
  m_sButtonLabels[MMS_GAME][GAME_LOAD_GAME] = XMLResources::GLOBAL.getCEGUIString("load_game");
#endif
  m_sButtonLabels[MMS_GAME][GAME_USER_GAME] = XMLResources::GLOBAL.getCEGUIString("select_map");
  m_sButtonLabels[MMS_GAME][GAME_BACK] = XMLResources::GLOBAL.getCEGUIString("back");

  // map editor
  setupButton(MMS_MAP_EDITOR, MAP_EDITOR_NEW_MAP, XMLResources::GLOBAL.getCEGUIString("new_map"), MMS_RESULT_NEW_MAP);
  setupButton(MMS_MAP_EDITOR, MAP_EDITOR_LOAD_MAP, XMLResources::GLOBAL.getCEGUIString("load_map"), MMS_MAP_EDITOR_SELECT_MAP);
  setupButton(MMS_MAP_EDITOR, MAP_EDITOR_BACK, XMLResources::GLOBAL.getCEGUIString("back"), MMS_GAME);

  setupButton(MMS_MAP_EDITOR_SELECT_MAP, MAP_EDITOR_LOAD_BACK, XMLResources::GLOBAL.getCEGUIString("back"), MMS_MAP_EDITOR);

  // load
  setupButton(MMS_LOAD_GAME, LOAD_GAME_BACK, XMLResources::GLOBAL.getCEGUIString("back"), MMS_GAME);

  // options
  m_iTargetState[MMS_OPTIONS][OPTIONS_VIDEO] = MMS_OPTIONS_VIDEO;
  m_iTargetState[MMS_OPTIONS][OPTIONS_INPUT] = MMS_OPTIONS_INPUT;
  m_iTargetState[MMS_OPTIONS][OPTIONS_BACK] = MMS_START;
  m_sButtonLabels[MMS_OPTIONS][OPTIONS_VIDEO] = XMLResources::GLOBAL.getCEGUIString("video");
  m_sButtonLabels[MMS_OPTIONS][OPTIONS_INPUT] = XMLResources::GLOBAL.getCEGUIString("input");
  m_sButtonLabels[MMS_OPTIONS][OPTIONS_BACK] = XMLResources::GLOBAL.getCEGUIString("back");


  m_iTargetState[MMS_OPTIONS_VIDEO][OPTIONS_VIDEO_BACK] = MMS_OPTIONS;
  m_sButtonLabels[MMS_OPTIONS_VIDEO][OPTIONS_VIDEO_BACK] = XMLResources::GLOBAL.getCEGUIString("back");

  m_iTargetState[MMS_OPTIONS_INPUT][OPTIONS_INPUT_BACK] = MMS_OPTIONS;
  m_sButtonLabels[MMS_OPTIONS_INPUT][OPTIONS_INPUT_BACK] = XMLResources::GLOBAL.getCEGUIString("back");

  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_BACK_TO_GAME] = MMS_RESULT_BACK_TO_GAME;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_BACK_TO_GAME] = XMLResources::GLOBAL.getCEGUIString("back_to_game");
  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_OPTIONS] = MMS_OPTIONS;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_OPTIONS] = XMLResources::GLOBAL.getCEGUIString("options");
  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_EXIT_GAME] = MMS_START;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_EXIT_GAME] = XMLResources::GLOBAL.getCEGUIString("exit_game");

  m_iTargetState[MMS_USER_GAME][USER_GAME_BACK] = MMS_GAME;
  m_sButtonLabels[MMS_USER_GAME][USER_GAME_BACK] = XMLResources::GLOBAL.getCEGUIString("back");

  // create cegui windows/buttons
  m_pMMRoot = pGUIRoot->createChild("DefaultWindow", "MainMenuRoot");
  m_pMMRoot->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  m_pMMRoot->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
  m_pMMRoot->setInheritsAlpha(false);
  m_pMMRoot->setAlpha(1);
  m_pMMRoot->setRiseOnClickEnabled(false);
  m_pMMRoot->moveToBack();

  CEGUI::Window *pBackground = m_pMMRoot->
    createChild("OgreTray/StaticImage", "Background");
  pBackground->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  pBackground->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
  pBackground->setInheritsAlpha(false);
  pBackground->setAlpha(1);
  pBackground->setProperty("Image", "main_menu_background/full");
  pBackground->setProperty("FrameEnabled", "False");
  pBackground->setProperty("BackgroundEnabled", "True");
  pBackground->setRiseOnClickEnabled(false);

  CEGUI::Window *pButtonContainer = m_pMMRoot->createChild("DefaultWindow", "ButtonContainer");
  m_pButtonContainer = pButtonContainer;
  pButtonContainer->setPosition(UVector2(UDim(0.2f, 0), UDim(0.5f, 0)));
  pButtonContainer->setSize(USize(UDim(0.5f, 0), UDim(0.4f, 0)));
  pButtonContainer->setRiseOnClickEnabled(false);

  for (int i = 0; i < NUM_SLOTS; i++) {
    m_vSlots[i] = pButtonContainer->createChild("OgreTray/Button", "Slot" + CEGUI::PropertyHelper<int>::toString(i));
    m_vSlots[i]->setSize(USize(UDim(0.4f, 0), UDim(0.2f, 0)));
    m_vSlots[i]->setPosition(UVector2(UDim(0, 0), UDim(0.25f * i, 0)));
    m_vSlots[i]->setRiseOnClickEnabled(false);
#ifdef INPUT_KEYBOARD_ONLY
    m_vSlots[i]->setAlpha(BUTTON_MIN_ALPHA);
#else
    m_vSlots[i]->setAlpha(1);
#endif
    m_vSlots[i]->setFont("dejavusans12");
    m_vSlots[i]->enable();
    m_vSlots[i]->subscribeEvent(
				CEGUI::PushButton::EventClicked,
				CEGUI::Event::Subscriber(&CMainMenu::buttonClicked, this));
    m_vSlots[i]->subscribeEvent(
				CEGUI::PushButton::EventMouseEntersArea,
				CEGUI::Event::Subscriber(&CMainMenu::buttonSelected, this));
    m_vSlots[i]->setVisible(false);
  }

#ifndef INPUT_KEYBOARD_ONLY
  m_pSelectButton = pButtonContainer->createChild("OgreTray/Button", "SelectButton");
  m_pSelectButton->setText("Start");
  m_pSelectButton->setFont("dejavusans12");
  m_pSelectButton->setPosition(UVector2(UDim(0.6, 0), UDim(0.75f, 0)));
  m_pSelectButton->setSize(USize(UDim(0.4f, 0), UDim(0.2f, 0)));
  m_pSelectButton->subscribeEvent(CEGUI::PushButton::EventClicked,
				  CEGUI::Event::Subscriber(&CMainMenu::onSelectButtonClicked, this));
#endif

  m_pSaveStatesWindow = dynamic_cast<CEGUI::Listbox*>(pButtonContainer->createChild("OgreTray/Listbox", "SaveStatesBox"));
  m_pSaveStatesWindow->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  m_pSaveStatesWindow->setSize(USize(UDim(0.6f, 0), UDim(0.6f, 0)));
  m_pSaveStatesWindow->setFont("dejavusans8");
  m_pSaveStatesWindow->setRiseOnClickEnabled(false);
#ifndef INPUT_KEYBOARD_ONLY
  m_pSaveStatesWindow->subscribeEvent(Listbox::EventSelectionChanged, Event::Subscriber(&CMainMenu::onSelectionChanged, this));
#endif

  m_pSaveStatePreviewWindow = pButtonContainer->createChild("OgreTray/StaticImage", "PreviewPricture");
  m_pSaveStatePreviewWindow->setPosition(UVector2(UDim(0.65f, 0), UDim(0.1f, 0)));
  m_pSaveStatePreviewWindow->setSize(USize(UDim(0.35f, 0), UDim(0.4f, 0)));
  m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/none");
  m_pSaveStatePreviewWindow->setRiseOnClickEnabled(false);

  // hide everything from start
  m_pSaveStatesWindow->setVisible(false);
  m_pSaveStatePreviewWindow->setVisible(false);

  hide();
}
CMainMenu::~CMainMenu() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CMainMenu::setupButton(MainMenu::EState eState, unsigned int uiButtonID, const CEGUI::String &sLabel, MainMenu::EState iTargetID) {
  m_sButtonLabels[eState][uiButtonID] = sLabel;
  m_iTargetState[eState][uiButtonID] = iTargetID;
}
void CMainMenu::createResources() {
  if (!m_pMMRoot->getChild("Background")) {
    throw Ogre::Exception(0, "Background child not found", __FILE__);
  }
  m_pMMRoot->getChild("Background")->setProperty("Image", "main_menu_background/full");
  for (int i = 0; i < NUM_SLOTS; i++) {
  }
}
void CMainMenu::update(Ogre::Real tpf) {
  if (!m_pMMRoot->isVisible()) { return; }

#ifdef INPUT_KEYBOARD_ONLY
  for (int i = 0; i < NUM_SLOTS; i++) {
    if (i == m_iSelectedSlot) {
      m_vSlots[i]->setAlpha(min(m_vSlots[i]->getAlpha() + tpf * BUTTON_ALPHA_CHANGE_PER_SEC, 1.0f));
    }
    else {
      m_vSlots[i]->setAlpha(max(m_vSlots[i]->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
    }
    if (m_bSaveListSelected){
      m_pSaveStatesWindow->setAlpha(min(m_pSaveStatesWindow->getAlpha() + tpf * BUTTON_ALPHA_CHANGE_PER_SEC, 1.0f));
      m_pSaveStatePreviewWindow->setAlpha(min(m_pSaveStatePreviewWindow->getAlpha() + tpf * BUTTON_ALPHA_CHANGE_PER_SEC, 1.0f));
      m_pMapInfoWindow->setAlpha(min(m_pSaveStatePreviewWindow->getAlpha() + tpf * BUTTON_ALPHA_CHANGE_PER_SEC, 1.0f));
    }
    else {
      m_pSaveStatesWindow->setAlpha(max(m_pSaveStatesWindow->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
      m_pSaveStatePreviewWindow->setAlpha(max(m_pSaveStatePreviewWindow->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
      m_pMapInfoWindow->setAlpha(max(m_pSaveStatePreviewWindow->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
    }
  }
#endif
  for (int i = 0; i < static_cast<int>(m_pSaveStatesWindow->getItemCount()); ++i) {
    if (m_iSelectedLoadState == i) {
      dynamic_cast<ListboxTextItem*>(m_pSaveStatesWindow->getListboxItemFromIndex(i))->setTextColours(Colour(1, 1, 1));
    }
    else {
      dynamic_cast<ListboxTextItem*>(m_pSaveStatesWindow->getListboxItemFromIndex(i))->setTextColours(Colour(0, 0, 0));
    }
  }
}
void CMainMenu::changeState(MainMenu::EState eState) {
  if (eState == MMS_OPTIONS && (m_eCurrentState == MMS_GAME_ESCAPE || m_eCurrentState == MMS_START)) {
    m_iTargetState[MMS_OPTIONS][OPTIONS_BACK] = m_eCurrentState;
  }

  // old state

  switch (m_eCurrentState) {
  case MMS_RESULT_NEW_GAME:
    break;
  case MMS_OPTIONS_VIDEO:
    m_pOptionPages[OPTIONS_VIDEO]->setVisible(false);
    CSettings::getSingleton().writeToFile();
    break;
  case MMS_OPTIONS_INPUT:
    m_pOptionPages[OPTIONS_INPUT]->setVisible(false);
    CSettings::getSingleton().writeToFile();
    break;
  case MMS_USER_GAME:
    break;
  default:
    break;
  }

  m_bSaveListSelected = false;
  m_eCurrentState = eState;
  m_iSelectedSlot = 0;
  m_iSelectedLoadState = 0;

  switch (eState) {
  case MMS_RESULT_NEW_GAME:
    break;
  case MMS_RESULT_LOAD_GAME:
    break;
  case MMS_RESULT_CREDITS:
    CGameState::getSingleton().changeGameState(CGameState::GS_CREDITS);
    break;
  case MMS_RESULT_EXIT:
    CGame::getSingleton().shutDown();
    break;
  case MMS_RESULT_BACK_TO_GAME:
    hide();
    break;
#if ENABLE_MAP_EDITOR
  case MMS_RESULT_NEW_MAP:
    CGameState::getSingleton().changeGameState(CGameState::GS_GAME, std::shared_ptr<CMapPack>(new CMapPack("new_map")));
    break;
#endif
  case MMS_OPTIONS_INPUT:
    CGUIManager::getSingleton().addGUIOverlay(new CGUIInputSettings(m_pMMRoot));
    m_eCurrentState = MMS_OPTIONS;
    break;
  case MMS_OPTIONS_VIDEO:
    CGUIManager::getSingleton().addGUIOverlay(new CGUIVideoSettings(m_pMMRoot));
    m_eCurrentState = MMS_OPTIONS;
    break;
  case MMS_USER_GAME:
    CGUIManager::getSingleton().addGUIOverlay(new CGUILevelSelect(m_pMMRoot));
    m_eCurrentState = MMS_START;
  default:
    for (int i = 0; i < NUM_SLOTS; i++) {
      if (m_iTargetState[m_eCurrentState][i] != MMS_STATE_NONE) {
        m_vSlots[i]->setVisible(true);
        m_vSlots[i]->setText(m_sButtonLabels[m_eCurrentState][i]);
#ifdef INPUT_KEYBOARD_ONLY
        m_vSlots[i]->setAlpha(BUTTON_MIN_ALPHA);
#endif
      }
      else {
        m_vSlots[i]->setVisible(false);
      }
    }
    for (int i = 0; i < NUM_SLOTS; i++) {
      if(m_iTargetState[m_eCurrentState][i] != MMS_STATE_NONE) {
        m_iSelectedSlot = i;
        break;
      }
    }
    break;
  }

  m_pSaveStatesWindow->setVisible(false);
  m_pSaveStatePreviewWindow->setVisible(false);
  if (eState == MMS_LOAD_GAME) {
    m_pSaveStatesWindow->setVisible(true);
    m_pSaveStatePreviewWindow->setVisible(true);
#ifndef INPUT_KEYBOARD_ONLY
    m_pSelectButton->setVisible(true);
#endif

    while (m_pSaveStatesWindow->getItemCount() > 0) {
      m_pSaveStatesWindow->removeItem(m_pSaveStatesWindow->getListboxItemFromIndex(0));
    }

    for (const CSaveState &state : CSaveStateManager::getSingleton().listSaveState()) {
      ListboxTextItem *pItem =
	new ListboxTextItem("Act " + PropertyHelper<int>::toString(state.getActID()) +
			    " Scene " + PropertyHelper<int>::toString(state.getSceneID()) + ": " +
			    PropertyHelper<int>::toString(state.getTime().tm_year) + "/" +
			    PropertyHelper<int>::toString(state.getTime().tm_mon) + "/" +
			    PropertyHelper<int>::toString(state.getTime().tm_mday) + " " +
			    PropertyHelper<int>::toString(state.getTime().tm_hour) + ":" +
			    PropertyHelper<int>::toString(state.getTime().tm_min) + ":" +
			    PropertyHelper<int>::toString(state.getTime().tm_sec)
			    );
      m_pSaveStatesWindow->addItem(pItem);
      pItem->setUserData(const_cast<CSaveState*>(&state));
    }

    selectedSaveStateChanged();
  }
  else if (eState == MMS_MAP_EDITOR_SELECT_MAP) {
    m_pSaveStatesWindow->setVisible(true);
    m_pSaveStatePreviewWindow->setVisible(false);
#ifndef INPUT_KEYBOARD_ONLY
    m_pSelectButton->setVisible(true);
#endif

    while (m_pSaveStatesWindow->getItemCount() > 0) {
      m_pSaveStatesWindow->removeItem(m_pSaveStatesWindow->getListboxItemFromIndex(0));
    }
    m_vUserFiles.clear();
    int i = 0;
    // reload resourcesresize height to content
    Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("level_user");
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("level_user");
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("level_user");
    Ogre::FileInfoListPtr ptr(Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo("level_user", "*.xml"));
    m_vUserFiles.resize(ptr->size());
    for (Ogre::FileInfo &file : *ptr.get()) {
      m_vUserFiles[i] = file.basename;
      ListboxTextItem *pItem = new ListboxTextItem(file.filename.substr(0, file.filename.rfind(".")));
      m_pSaveStatesWindow->addItem(pItem);
      i++;
    }

    selectedSaveStateChanged();
  }
  else {
    m_pSaveStatesWindow->setVisible(false);
    m_pSaveStatePreviewWindow->setVisible(false);
#ifndef INPUT_KEYBOARD_ONLY
    m_pSelectButton->setVisible(false);
#endif
  }

  if (eState == MMS_START) {
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU);
    show();
  }
}
bool CMainMenu::keyPressed(const OIS::KeyEvent &arg) {
  switch (arg.key) {
#ifdef INPUT_KEYBOARD_ONLY
  case OIS::KC_DOWN:
    if (m_bSaveListSelected) {
        ++m_iSelectedLoadState;
        if (m_iSelectedLoadState >= static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
          m_iSelectedLoadState = m_pSaveStatesWindow->getItemCount() - 1;
        }
        selectedSaveStateChanged();
    }
    else {
      ++m_iSelectedSlot;
      while (m_iSelectedSlot >= NUM_SLOTS || m_iTargetState[m_eCurrentState][m_iSelectedSlot] == MMS_STATE_NONE) {
        if (m_iSelectedSlot >= NUM_SLOTS) {
          m_iSelectedSlot = 0;
        }
        else {
          ++m_iSelectedSlot;
        }
      }
    }
    break;
  case OIS::KC_UP:
    if (m_bSaveListSelected) {
        --m_iSelectedLoadState;
        if (m_iSelectedLoadState < 0) {
          m_iSelectedLoadState = 0;
        }
        selectedSaveStateChanged();
    }
    else {
      --m_iSelectedSlot;
      while (m_iSelectedSlot < 0 || m_iTargetState[m_eCurrentState][m_iSelectedSlot] == MMS_STATE_NONE) {
        if (m_iSelectedSlot < 0) {
          m_iSelectedSlot = NUM_SLOTS - 1;
        }
        else {
          --m_iSelectedSlot;
        }
      }
    }
    break;
  case OIS::KC_RETURN:
    if (m_bSaveListSelected) {
      activateLoadState();
    }
    else {
      changeState(m_iTargetState[m_eCurrentState][m_iSelectedSlot]);
    }
    break;
  case OIS::KC_TAB:
    if (m_eCurrentState == MMS_LOAD_GAME || m_eCurrentState == MMS_USER_GAME) {
      m_bSaveListSelected = !m_bSaveListSelected;
      if (m_bSaveListSelected) {
        m_iSelectedSlot = -1;
      }
      else {
	if (m_eCurrentState == MMS_LOAD_GAME) {
	  m_iSelectedSlot = LOAD_GAME_BACK;
	}
	else if (m_eCurrentState == MMS_USER_GAME) {
	  m_iSelectedSlot = USER_GAME_BACK;
	}
      }
    }
    break;
#endif
  default:
    break;
  }
  return true;
}
void CMainMenu::selectedSaveStateChanged() {
  Ogre::LogManager::getSingleton().logMessage("Save state " + Ogre::StringConverter::toString(m_iSelectedLoadState) + " selected.");
  if (m_eCurrentState == MMS_LOAD_GAME) {
    if (m_iSelectedLoadState < 0 || m_iSelectedLoadState >= static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
      m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/none");
      return;
    }
    m_pStateToLoad = static_cast<const CSaveState*>(m_pSaveStatesWindow->getListboxItemFromIndex(m_iSelectedLoadState)->getUserData());
    m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/" + PropertyHelper<int>::toString(m_pStateToLoad->getActID()) + "-" + PropertyHelper<int>::toString(m_pStateToLoad->getSceneID()));
  }
  m_pSaveStatesWindow->ensureItemIsVisible(m_iSelectedLoadState);
  m_pSaveStatesWindow->setItemSelectState(m_iSelectedLoadState, true);
}
bool CMainMenu::buttonSelected(const CEGUI::EventArgs& args) {
  CEGUI::Window *pBtn = dynamic_cast<const WindowEventArgs*>(&args)->window;
  for (unsigned int i = 0; i < NUM_SLOTS; i++) {
    if (m_vSlots[i] == pBtn) {
      m_iSelectedSlot = i;
      break;
    }
  }
  return true;
}
bool CMainMenu::buttonClicked(const CEGUI::EventArgs& args) {
  buttonSelected(args);
  unsigned int uiSelectedSlotBuffer = m_iSelectedSlot;
  changeState(m_iTargetState[m_eCurrentState][m_iSelectedSlot]);
  m_iSelectedSlot = uiSelectedSlotBuffer;
  return true;
}
bool CMainMenu::saveStateListEntered(const CEGUI::EventArgs&) {
  m_bSaveListSelected = true;
  m_iSelectedSlot = -1;
  return true;
}
bool CMainMenu::saveStateListLeaved(const CEGUI::EventArgs&) {
  m_bSaveListSelected = false;
  m_iSelectedSlot = LOAD_GAME_BACK;
  return true;
}
bool CMainMenu::saveStateListMouseMoved(const CEGUI::EventArgs& args) {
    /*const CEGUI::MouseEventArgs *mea = dynamic_cast<const CEGUI::MouseEventArgs*>(&args);
  CEGUI::ListboxItem *pItem = m_pSaveStatesWindow->getItemAtPoint(mea->position);
  if (!pItem) {return true;}

  m_iSelectedLoadState = m_pSaveStatesWindow->getItemIndex(pItem);
  selectedSaveStateChanged();*/
  return true;
}
bool CMainMenu::onSelectionChanged(const CEGUI::EventArgs& args) {
  if (m_pSaveStatesWindow->getFirstSelectedItem()) {
    m_iSelectedLoadState = m_pSaveStatesWindow->getItemIndex(m_pSaveStatesWindow->getFirstSelectedItem());
    selectedSaveStateChanged();
  }
  return true;
}
void CMainMenu::activateLoadState() {
if (m_iSelectedLoadState >= 0
      && m_iSelectedLoadState < static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
    if (m_eCurrentState == MMS_LOAD_GAME) {
      m_pStateToLoad
	= static_cast<const CSaveState*>(m_pSaveStatesWindow->getListboxItemFromIndex(m_iSelectedLoadState)->getUserData());
    }
    else if (m_eCurrentState == MMS_USER_GAME) {
      m_pStateToLoad = NULL;
    }
    changeState(MMS_RESULT_LOAD_GAME);
  }
}
void CMainMenu::resizeGUI(Ogre::Real fScaling) {
  // if 0, then smallest size
  // if 1, then larges size
  m_pButtonContainer->
    setPosition(MathUtil::linearInterpolate(UVector2(UDim(0.2f, 0),
						     UDim(0.5f, 0)),
					    UVector2(UDim(0.1f, 0),
						     UDim(0.1f, 0)),
					    fScaling));
  m_pButtonContainer->
    setSize(MathUtil::linearInterpolate(USize(UDim(0.5f, 0),
					      UDim(0.4f, 0)),
					USize(UDim(0.8f, 0),
					      UDim(0.8f, 0)),
					fScaling));

  CEGUI::String smallfont("dejavusans8");
  CEGUI::String bigfont("dejavusans12");
  if (fScaling > 0.667f) {
    smallfont = "dejavusans20";
  }
  else if (fScaling > 0.333f) {
    smallfont = "dejavusans12";
  }

  if (fScaling > 0.5f) {
    bigfont = "dejavusans20";
  }

  for (int i = 0; i < NUM_SLOTS; i++) {
    m_vSlots[i]->setFont(bigfont);
  }
  m_pSaveStatesWindow->setFont(smallfont);

#ifndef INPUT_KEYBOARD_ONLY
  m_pSelectButton->setFont(bigfont);
#endif
}

void CMainMenu::windowSizeChanged(const CEGUI::Sizef &vSize) {
  Slider *pSlider = NULL;
#if ENABLE_MAP_EDITOR
  pSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_INPUT]->getChild("MapEditorButtonSizeSlider"));
  pSlider->setMaxValue(min(vSize.d_height / 4.0, vSize.d_width / 8.0));
#endif
}
bool CMainMenu::onSelectButtonClicked(const CEGUI::EventArgs &args) {
  activateLoadState();
  return true;
}
void CMainMenu::show() {
  m_pMMRoot->setVisible(true);
  setInputListenerEnabled(true);
  pause(PAUSE_ALL);
}
void CMainMenu::hide() {
  m_pMMRoot->setVisible(false);
  setInputListenerEnabled(false);
  unpause(PAUSE_ALL);
}
