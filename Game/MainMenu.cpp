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
#include "LevelState.hpp"
#include "XMLResources/Manager.hpp"

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
  : m_pLevelInfo(NULL),
    m_vSlots(NUM_SLOTS),
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

  for (int i = 0; i < NUM_SLOTS; i++) {
    m_vSlots[i] = pButtonContainer->createChild("OgreTray/Button", "Slot" + CEGUI::PropertyHelper<int>::toString(i));
    m_vSlots[i]->setSize(USize(UDim(0.4f, 0), UDim(0.2f, 0)));
    m_vSlots[i]->setPosition(UVector2(UDim(0, 0), UDim(0.25f * i, 0)));
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
#ifndef INPUT_KEYBOARD_ONLY
  m_pSaveStatesWindow->subscribeEvent(Listbox::EventSelectionChanged, Event::Subscriber(&CMainMenu::onSelectionChanged, this));
#endif

  m_pSaveStatePreviewWindow = pButtonContainer->createChild("OgreTray/StaticImage", "PreviewPricture");
  m_pSaveStatePreviewWindow->setPosition(UVector2(UDim(0.65f, 0), UDim(0.1f, 0)));
  m_pSaveStatePreviewWindow->setSize(USize(UDim(0.35f, 0), UDim(0.4f, 0)));
  m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/none");

  ScrollablePane *pMapInfoContainer = dynamic_cast<ScrollablePane*>(pButtonContainer->createChild("OgreTray/ScrollablePane", "MapInfoContainer"));
  pMapInfoContainer->setShowHorzScrollbar(false);
  pMapInfoContainer->setPosition(UVector2(UDim(0.6f, 0), UDim(0, 0)));
  pMapInfoContainer->setSize(USize(UDim(0.4f, 0), UDim(0.6f, 0)));
  m_pMapInfoContainer = pMapInfoContainer;

  m_pMapInfoWindow = pMapInfoContainer->createChild("OgreTray/StaticText", "MapInfo");
  m_pMapInfoWindow->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  m_pMapInfoWindow->setSize(USize(UDim(1, 0), UDim(2, 0)));
  m_pMapInfoWindow->setText("Difficulty: easy");
  m_pMapInfoWindow->setProperty("VertFormatting", "TopAligned");

  m_pMapInfoContainer->setVisible(false);
  
  // option pages
  // ======================
  
  // input
  float fHeight = 0;
  m_pOptionPages[OPTIONS_INPUT] = pButtonContainer->createChild("OgreTray/Group", "InputOptionsContainer");
  m_pOptionPages[OPTIONS_INPUT]->setText(XMLResources::GLOBAL.getCEGUIString("input"));
  m_pOptionPages[OPTIONS_INPUT]->setSize(USize(UDim(1, 0), UDim(0.7, 0)));


#if ENABLE_MAP_EDITOR
  Window *pMapEditorButtonSizeText = m_pOptionPages[OPTIONS_INPUT]->createChild("OgreTray/StaticText", "MapEditorButtonSliderText");
  pMapEditorButtonSizeText->setPosition(UVector2(UDim(0, 0), UDim(0.00, 0)));
  pMapEditorButtonSizeText->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pMapEditorButtonSizeText->setProperty("FrameEnabled", "False");
  pMapEditorButtonSizeText->setProperty("BackgroundEnabled", "False");
  Slider * pMapEditorButtonSizeSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_INPUT]->createChild("OgreTray/Slider", "MapEditorButtonSizeSlider"));
  pMapEditorButtonSizeSlider->setPosition(UVector2(UDim(0, 0), UDim(0.12, 0)));
  pMapEditorButtonSizeSlider->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pMapEditorButtonSizeSlider->setMaxValue(300);
  pMapEditorButtonSizeSlider->setClickStep(20);
  pMapEditorButtonSizeSlider
    ->subscribeEvent(Slider::EventValueChanged,
		     Event::Subscriber(&CMainMenu::mapEditorButtonSizeSliderValueChanged, this));
  // this will also set the initial value for the touch buttons
  pMapEditorButtonSizeSlider->setCurrentValue(CSettings::getSingleton().getInputSettings().m_fMapEditorButtonSize);
  fHeight += 0.25;
#endif
#ifdef INPUT_TOUCH
  Window *pButtonSizeText = m_pOptionPages[OPTIONS_INPUT]->createChild("OgreTray/StaticText", "ButtonSliderText");
  pButtonSizeText->setPosition(UVector2(UDim(0, 0), UDim(fHeight, 0)));
  pButtonSizeText->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pButtonSizeText->setProperty("FrameEnabled", "False");
  pButtonSizeText->setProperty("BackgroundEnabled", "False");
  Slider * pButtonSizeSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_INPUT]->createChild("OgreTray/Slider", "ButtonSizeSlider"));
  pButtonSizeSlider->setPosition(UVector2(UDim(0, 0), UDim(fHeight + 0.12, 0)));
  pButtonSizeSlider->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pButtonSizeSlider->setMaxValue(300);
  pButtonSizeSlider->setClickStep(20);
  pButtonSizeSlider->subscribeEvent(Slider::EventValueChanged,
				    Event::Subscriber(&CMainMenu::buttonSizeSliderValueChanged,
						      this));
  // this will also set the initial value for the touch buttons
  pButtonSizeSlider->setCurrentValue(CSettings::getSingleton().getInputSettings().m_fTouchButtonSize);
  fHeight += 0.25;
#endif

  m_pOptionPages[OPTIONS_INPUT]->setVisible(false);

  // video
  m_pOptionPages[OPTIONS_VIDEO] = pButtonContainer->createChild("OgreTray/Group", "VideoOptionsContainer");
  m_pOptionPages[OPTIONS_VIDEO]->setText(XMLResources::GLOBAL.getCEGUIString("video"));
  m_pOptionPages[OPTIONS_VIDEO]->setSize(USize(UDim(1, 0), UDim(0.7, 0)));
  Window *pMenuSizeText = m_pOptionPages[OPTIONS_VIDEO]->createChild("OgreTray/StaticText", "MenuSizeText");
  pMenuSizeText->setPosition(UVector2(UDim(0, 0), UDim(0.00, 0)));
  pMenuSizeText->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pMenuSizeText->setProperty("FrameEnabled", "False");
  pMenuSizeText->setProperty("BackgroundEnabled", "False");
  Slider * pMenuSizeSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_VIDEO]->createChild("OgreTray/Slider", "MenuSizeSlider"));
  pMenuSizeSlider->setPosition(UVector2(UDim(0, 0), UDim(0.12, 0)));
  pMenuSizeSlider->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pMenuSizeSlider->setMaxValue(1);
  pMenuSizeSlider->setClickStep(0.1);
  pMenuSizeSlider->setCurrentValue(0.24152342); // to force call of event
  pMenuSizeSlider->subscribeEvent(Slider::EventValueChanged,
				    Event::Subscriber(&CMainMenu::menuSizeSliderValueChanged,
						      this));
  // initial value
  pMenuSizeSlider->setCurrentValue(CSettings::getSingleton().getVideoSettings().m_fHUDSize);
  
  m_pOptionPages[OPTIONS_VIDEO]->setVisible(false);


  // level selection
  // ===============
  m_pLevelSelection = m_pButtonContainer->createChild("OgreTray/Group",
						      "LevelSelection");
  m_pLevelSelection->setSize(USize(UDim(1, 0), UDim(0.7, 0)));
  m_pLevelSelection->setText(XMLResources::GLOBAL.getCEGUIString("select_level"));

  ScrollablePane *pLevelPane = dynamic_cast<ScrollablePane*>(m_pLevelSelection->createChild("OgreTray/ScrollablePane", "Pane"));
  pLevelPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pLevelPane->setSize(USize(UDim(0.7, 0), UDim(1, 0)));
 
  Window *pLevelInfoWindow = m_pLevelSelection->createChild("OgreTray/ScrollablePane", "Info");
  pLevelInfoWindow->setPosition(UVector2(UDim(0.7, 0), UDim(0, 0)));
  pLevelInfoWindow->setSize(USize(UDim(0.3, 0), UDim(1, 0)));

  Window *pLevelInfoText = pLevelInfoWindow->createChild("OgreTray/StaticText", "Text");
  pLevelInfoText->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pLevelInfoText->setSize(USize(UDim(1, 0), UDim(3, 0)));
  pLevelInfoText->setProperty("VertFormatting", "TopAligned");
  pLevelInfoText->setProperty("BackgroundEnabled", "False");
  pLevelInfoText->setProperty("FrameEnabled", "False");

  Window *pChickenButton = m_pButtonContainer->createChild("OgreTray/ImageButton",
						     "ChickenButton");
  pChickenButton->setPosition(UVector2(UDim(0.45, 0), UDim(0.75, 0)));
  pChickenButton->setSize(USize(UDim(0.15, 0), UDim(0.2, 0)));
  pChickenButton->setText("0");
  pChickenButton->setProperty("Image", "hud_weapons/skip");
  pChickenButton->subscribeEvent(PushButton::EventClicked, 
				 Event::Subscriber(&CMainMenu::onChickenPressed, this));

  m_pLevelSelection->setVisible(false);

  // hide everything from start
  m_pMapInfoContainer->setVisible(false);
  m_pLevelSelection->setVisible(false);
  m_pButtonContainer->getChild("ChickenButton")->setVisible(false);
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
    if (eState == MMS_RESULT_LOAD_GAME ||
	eState == MMS_RESULT_NEW_GAME)  {
      assert(m_pLevelInfo);
      m_pMapInfo = shared_ptr<CMapInfo>(new CMapInfo(m_pLevelInfo->sLevelFileName,
						     "level_user"));
    }
    m_pLevelInfo = NULL;
    break;
  default:
    break;
  }

  m_bSaveListSelected = false;
  m_eCurrentState = eState;
  m_iSelectedSlot = 0;
  m_iSelectedLoadState = 0;

  
  switch (m_eCurrentState) {
 case MMS_OPTIONS_VIDEO:
   m_pOptionPages[OPTIONS_VIDEO]->setVisible(true);
   break;
 case MMS_OPTIONS_INPUT:
   m_pOptionPages[OPTIONS_INPUT]->setVisible(true);
   break;
  default:
    break;
  }

  switch (eState) {
  case MMS_RESULT_NEW_GAME:
    CGameState::getSingleton().changeGameState(CGameState::GS_GAME);
    break;
  case MMS_RESULT_LOAD_GAME:
    if (m_pStateToLoad) {
      CGameState::getSingleton().changeGameState(CGameState::GS_GAME, m_pStateToLoad);
    }
    else if (m_pMapInfo) {
      CGameState::getSingleton().changeGameState(CGameState::GS_GAME, m_pMapInfo);
    }
    else {
      CGameState::getSingleton().changeGameState(CGameState::GS_GAME);
    }
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
    CGameState::getSingleton().changeGameState(CGameState::GS_GAME, std::shared_ptr<CMapInfo>(new CMapInfo()));
    break;
#endif
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

  m_pMapInfoContainer->setVisible(false);
  m_pLevelSelection->setVisible(false);
  m_pButtonContainer->getChild("ChickenButton")->setVisible(false);
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
    m_pMapInfoContainer->setVisible(true);
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
  else if (eState == MMS_USER_GAME) {
    m_pSaveStatesWindow->setVisible(false);
    m_pSaveStatePreviewWindow->setVisible(false);
    m_pLevelSelection->setVisible(true);
    m_pButtonContainer->getChild("ChickenButton")->setVisible(true);
#ifndef INPUT_KEYBOARD_ONLY
    m_pSelectButton->setVisible(true);
#endif

    updateLevelsSelection();
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
  else if (m_eCurrentState == MMS_USER_GAME || m_eCurrentState == MMS_MAP_EDITOR_SELECT_MAP) {
    if (m_iSelectedLoadState < 0 || m_iSelectedLoadState >= static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
      m_pMapInfoWindow->setText("");
      return;
    }
    m_pMapInfo = std::shared_ptr<CMapInfo>(new CMapInfo(m_vUserFiles[m_iSelectedLoadState], "level_user"));
    m_pMapInfoWindow->setText(m_pMapInfo->generateInfoText());
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
  if (m_pLevelInfo) {
    changeState(MMS_RESULT_LOAD_GAME);
  }
  else if (m_iSelectedLoadState >= 0
      && m_iSelectedLoadState < static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
    if (m_eCurrentState == MMS_LOAD_GAME) {
      m_pStateToLoad
	= static_cast<const CSaveState*>(m_pSaveStatesWindow->getListboxItemFromIndex(m_iSelectedLoadState)->getUserData());
      m_pMapInfo.reset();
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
  m_pMapInfoWindow->setFont(smallfont);
  m_pOptionPages[OPTIONS_INPUT]->setFont(bigfont);
#ifdef INPUT_TOUCH
  m_pOptionPages[OPTIONS_INPUT]->getChild("ButtonSliderText")->setFont(smallfont);
#endif
#if ENABLE_MAP_EDITOR
  m_pOptionPages[OPTIONS_INPUT]->getChild("MapEditorButtonSliderText")->setFont(smallfont);
#endif
  m_pOptionPages[OPTIONS_VIDEO]->setFont(bigfont);
  m_pOptionPages[OPTIONS_VIDEO]->getChild("MenuSizeText")->setFont(smallfont);

#ifndef INPUT_KEYBOARD_ONLY
  m_pSelectButton->setFont(bigfont);
#endif
}
bool CMainMenu::mapEditorButtonSizeSliderValueChanged(const CEGUI::EventArgs& args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  const Slider *pSlider = dynamic_cast<const Slider*>(wndArgs.window);

  m_pOptionPages[OPTIONS_INPUT]->getChild("MapEditorButtonSliderText")->setText("MapEditor touch button size: " + PropertyHelper<int>::toString(pSlider->getCurrentValue()) + " px");

  CMapEditor::getSingleton().resize(pSlider->getCurrentValue());
  CSettings::getSingleton().getInputSettings().m_fMapEditorButtonSize = pSlider->getCurrentValue();

  return true;
} 
bool CMainMenu::buttonSizeSliderValueChanged(const EventArgs &args) {
#ifdef INPUT_TOUCH
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  const Slider *pSlider = dynamic_cast<const Slider*>(wndArgs.window);

  m_pOptionPages[OPTIONS_INPUT]->getChild("ButtonSliderText")->setText("Input touch button size: " + PropertyHelper<int>::toString(pSlider->getCurrentValue()) + " px");

  CGUIManager::getSingleton().changeTouchButtonSize(pSlider->getCurrentValue());
  CSettings::getSingleton().getInputSettings().m_fTouchButtonSize = pSlider->getCurrentValue();
#endif
  
  return true;
}
bool CMainMenu::menuSizeSliderValueChanged(const EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  const Slider *pSlider = dynamic_cast<const Slider*>(wndArgs.window);

  m_pOptionPages[OPTIONS_VIDEO]->getChild("MenuSizeText")->setText("Main menu size: " + PropertyHelper<int>::toString(pSlider->getCurrentValue() * 100) + " %");

  resizeGUI(pSlider->getCurrentValue());
  CSettings::getSingleton().getVideoSettings().m_fHUDSize = pSlider->getCurrentValue();
  
  return true;
}
void CMainMenu::windowSizeChanged(const CEGUI::Sizef &vSize) {
  Slider *pSlider = NULL;
#if ENABLE_MAP_EDITOR
  pSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_INPUT]->getChild("MapEditorButtonSizeSlider"));
  pSlider->setMaxValue(min(vSize.d_height / 4.0, vSize.d_width / 8.0));
#endif
#ifdef INPUT_TOUCH
  pSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_INPUT]->getChild("ButtonSizeSlider"));
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
void CMainMenu::updateLevelsSelection() {
  int iLeftChickens = 3; // maximum number of chickens
  
  ScrollablePane *pPane = dynamic_cast<ScrollablePane*>(m_pLevelSelection->getChild("Pane"));
  pPane->getHorzScrollbar()->setVisible(false);
  pPane->setShowHorzScrollbar(false);
  const Window *pContent = pPane->getContentPane();
  for (int i = pContent->getChildCount() - 1; i >= 0; i--) {
    pContent->getChildAtIdx(i)->destroy();
  }

  m_LevelList.load();
  const std::list<SLevelInfo> &lList(m_LevelList.getLevelInfoList());

  unsigned int uiLevelsPerRow = 5;
  float fButtonSize = pContent->getPixelSize().d_width / uiLevelsPerRow;
  std::list<SLevelInfo>::const_iterator it = lList.cbegin();
  std::string sPreviousLevelFileName;
  const SLevelInfo *pPreviousLevelInfo(NULL);
  RadioButton *pLastEnabled = NULL;
  for (unsigned int i = 0; i < lList.size(); i++) {
    RadioButton *pBut = dynamic_cast<RadioButton*>(pPane->createChild("OgreTray/ToggleRadioButton", PropertyHelper<unsigned int>::toString(i + 1)));
    pBut->setText(pBut->getName());
    pBut->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
    pBut->setPosition(UVector2(UDim(0, fButtonSize * (i % uiLevelsPerRow)), UDim(0, fButtonSize * (i / uiLevelsPerRow)))); 
    pBut->setUserData(const_cast<SLevelInfo*>(&(*it)));
    //pBut->setProperty("HorzFormatting", "CenterAligned");
    pBut->subscribeEvent(RadioButton::EventSelectStateChanged,
			 Event::Subscriber(&CMainMenu::onLevelButtonClicked,
					   this));
    pBut->setGroupID(1249845902);
    pBut->setSelected(false);
    if (i > 0) {
      if (CLevelState::has(it->sLevelFileName) &&
	  CLevelState::get(it->sLevelFileName).eMissionState == MS_SKIPPED) {
	pBut->setEnabled(true);
	pBut->setProperty("Image", "hud_weapons/skip");
	--iLeftChickens;
      }
      else {
	pBut->setProperty("Image", "");
	if (it->bTutorial) {
	  CLevelState::get(it->sLevelFileName, true);
	  pBut->setEnabled(true);
	}
	else if ((CLevelState::has(sPreviousLevelFileName) && 
		 CLevelState::get(sPreviousLevelFileName).eMissionState
		  != MS_FAILED) ||
		 (pPreviousLevelInfo && pPreviousLevelInfo->bTutorial)) {
	  pLastEnabled = pBut;
	  pBut->setEnabled(true);
	}
	else {
	  pBut->setEnabled(false);
	  pBut->setText("");
	  pBut->setProperty("Image", "hud_weapons/lock");
	}
      }
    }
    else {
      pLastEnabled = pBut;
    }
    sPreviousLevelFileName = it->sLevelFileName;
    pPreviousLevelInfo = &(*it);
    it++;
  }
  
  if (pLastEnabled) {
    pLastEnabled->setSelected(true);
  }


  m_pButtonContainer->getChild("ChickenButton")->setText(PropertyHelper<int>::toString(iLeftChickens));
  m_pButtonContainer->getChild("ChickenButton")->setEnabled(iLeftChickens > 0);
  //selectLevel(1);
}
void CMainMenu::selectLevel(unsigned int id) {
  m_uiSelectedLevelID = id;
  ScrollablePane *pPane = dynamic_cast<ScrollablePane*>(m_pLevelSelection->getChild("Pane"));
  ScrollablePane *pInfoPane = dynamic_cast<ScrollablePane*>(m_pLevelSelection->getChild("Info"));
  pPane->setShowHorzScrollbar(false);
  Window *pText = pInfoPane->getChild("Text");
  Window *pBut = pPane->getChild(PropertyHelper<unsigned int>::toString(id));

  SLevelInfo *pLevelInfo = static_cast<SLevelInfo*>(pBut->getUserData());
  pText->setText(pLevelInfo->sFullInfoText.c_str());
  
  pInfoPane->getVertScrollbar()->setScrollPosition(0);
  pInfoPane->getHorzScrollbar()->setVisible(false);
  m_pLevelInfo = pLevelInfo;
}
bool CMainMenu::onLevelButtonClicked(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs(dynamic_cast<const WindowEventArgs &>(args));
  Ogre::LogManager::getSingleton().logMessage(Ogre::String("Level ") + wndArgs.window->getName().c_str() + Ogre::String(" selected"));
  selectLevel(PropertyHelper<unsigned int>::fromString(wndArgs.window->getName()));
  return true;
}
bool CMainMenu::onChickenPressed(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs(dynamic_cast<const WindowEventArgs &>(args));
  unsigned int uiSelectedLevelID = m_uiSelectedLevelID;
  if (PropertyHelper<int>::fromString(wndArgs.window->getText()) <= 0) {
    // no more chicken available
    return true;
  }

  ScrollablePane *pPane = dynamic_cast<ScrollablePane*>(m_pLevelSelection->getChild("Pane"));
  Window *pBut = pPane->getChild(PropertyHelper<unsigned int>::toString(m_uiSelectedLevelID));

  SLevelInfo *pLevelInfo = static_cast<SLevelInfo*>(pBut->getUserData());
  SStatistics &stats(CLevelState::get(pLevelInfo->sLevelFileName, true));
  if (stats.eMissionState == MS_FAILED && !pLevelInfo->bTutorial) {
    stats.eMissionState = MS_SKIPPED;
    CLevelState::add(stats);
  }
  else {
    return true;
  }
 
  // update levels
  updateLevelsSelection();
  try {
    RadioButton *pRadio = dynamic_cast<RadioButton*>(pPane->getChild(PropertyHelper<unsigned int>::toString(uiSelectedLevelID + 1)));
    if (pRadio) {
      pRadio->setSelected(true);
    }
  }
  catch (...) {
    // child not found, end of map
  }
  return true;
}
