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

using namespace std;
using namespace CEGUI;

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
  m_iTargetState[MMS_START][START_EXIT] = MMS_RESULT_EXIT;
  m_sButtonLabels[MMS_START][START_START_GAME] = "Start game";
  m_sButtonLabels[MMS_START][START_OPTIONS] = "Options";
  m_sButtonLabels[MMS_START][START_EXIT] = "Exit";

  m_iTargetState[MMS_GAME][GAME_NEW_GAME] = MMS_RESULT_NEW_GAME;
  m_iTargetState[MMS_GAME][GAME_LOAD_GAME] = MMS_LOAD_GAME;
  m_iTargetState[MMS_GAME][GAME_BACK] = MMS_START;
  m_sButtonLabels[MMS_GAME][GAME_NEW_GAME] = "New game";
  m_sButtonLabels[MMS_GAME][GAME_LOAD_GAME] = "Load game";
  m_sButtonLabels[MMS_GAME][GAME_BACK] = "Back";

  m_iTargetState[MMS_LOAD_GAME][LOAD_GAME_BACK] = MMS_GAME;
  m_sButtonLabels[MMS_LOAD_GAME][LOAD_GAME_BACK] = "Back";

  m_iTargetState[MMS_OPTIONS][OPTIONS_VIDEO] = MMS_OPTIONS_VIDEO;
  m_iTargetState[MMS_OPTIONS][OPTIONS_INPUT] = MMS_OPTIONS_INPUT;
  m_iTargetState[MMS_OPTIONS][OPTIONS_BACK] = MMS_START;
  m_sButtonLabels[MMS_OPTIONS][OPTIONS_VIDEO] = "Video";
  m_sButtonLabels[MMS_OPTIONS][OPTIONS_INPUT] = "Input";
  m_sButtonLabels[MMS_OPTIONS][OPTIONS_BACK] = "Back";


  m_iTargetState[MMS_OPTIONS_VIDEO][OPTIONS_VIDEO_BACK] = MMS_OPTIONS;
  m_sButtonLabels[MMS_OPTIONS_VIDEO][OPTIONS_VIDEO_BACK] = "Back";

  m_iTargetState[MMS_OPTIONS_INPUT][OPTIONS_INPUT_BACK] = MMS_OPTIONS;
  m_sButtonLabels[MMS_OPTIONS_INPUT][OPTIONS_INPUT_BACK] = "Back";

  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_BACK_TO_GAME] = MMS_RESULT_BACK_TO_GAME;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_BACK_TO_GAME] = "Back to game";
  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_OPTIONS] = MMS_OPTIONS;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_OPTIONS] = "Options";
  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_EXIT_GAME] = MMS_START;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_EXIT_GAME] = "Exit game";

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
  }

  m_pSaveStatesWindow = dynamic_cast<CEGUI::Listbox*>(pButtonContainer->createChild("OgreTray/Listbox", "SaveStatesBox"));
  m_pSaveStatesWindow->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  m_pSaveStatesWindow->setSize(USize(UDim(0.6f, 0), UDim(0.6f, 0)));
  m_pSaveStatesWindow->setFont("dejavusans8");
  m_pSaveStatesWindow->subscribeEvent(
				      CEGUI::Window::EventMouseEntersArea,
				      CEGUI::Event::Subscriber(
							       &CMainMenu::saveStateListEntered,
							       this));
  m_pSaveStatesWindow->subscribeEvent(
				      CEGUI::Window::EventMouseLeavesArea,
				      CEGUI::Event::Subscriber(
							       &CMainMenu::saveStateListLeaved,
							       this));
  m_pSaveStatesWindow->subscribeEvent(
				      CEGUI::Window::EventMouseMove,
				      CEGUI::Event::Subscriber(
							       &CMainMenu::saveStateListMouseMoved,
							       this));
  m_pSaveStatesWindow->subscribeEvent(
				      CEGUI::Window::EventMouseClick,
				      CEGUI::Event::Subscriber(
							       &CMainMenu::saveStateListMouseClicked,
							       this));

  m_pSaveStatePreviewWindow = pButtonContainer->createChild("OgreTray/StaticImage", "PreviewPricture");
  m_pSaveStatePreviewWindow->setPosition(UVector2(UDim(0.65f, 0), UDim(0.1f, 0)));
  m_pSaveStatePreviewWindow->setSize(USize(UDim(0.35f, 0), UDim(0.4f, 0)));
  m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/none");


  // option pages
  // ======================
  
  // input
  m_pOptionPages[OPTIONS_INPUT] = pButtonContainer->createChild("OgreTray/Group", "InputOptionsContainer");
  m_pOptionPages[OPTIONS_INPUT]->setText("Input");
  m_pOptionPages[OPTIONS_INPUT]->setSize(USize(UDim(1, 0), UDim(0.7, 0)));
#ifdef INPUT_TOUCH
  Window *pButtonSizeText = m_pOptionPages[OPTIONS_INPUT]->createChild("OgreTray/StaticText", "ButtonSliderText");
  pButtonSizeText->setPosition(UVector2(UDim(0, 0), UDim(0.00, 0)));
  pButtonSizeText->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pButtonSizeText->setProperty("FrameEnabled", "False");
  pButtonSizeText->setProperty("BackgroundEnabled", "False");
  Slider * pButtonSizeSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_INPUT]->createChild("OgreTray/Slider", "ButtonSizeSlider"));
  pButtonSizeSlider->setPosition(UVector2(UDim(0, 0), UDim(0.12, 0)));
  pButtonSizeSlider->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pButtonSizeSlider->setMaxValue(300);
  pButtonSizeSlider->setClickStep(20);
  pButtonSizeSlider->subscribeEvent(Slider::EventValueChanged,
				    Event::Subscriber(&CMainMenu::buttonSizeSliderValueChanged,
						      this));
  // this will also set the initial value for the touch buttons
  pButtonSizeSlider->setCurrentValue(CSettings::getSingleton().getInputSettings().m_fTouchButtonSize);
#endif

  m_pOptionPages[OPTIONS_INPUT]->setVisible(false);

  // video
  m_pOptionPages[OPTIONS_VIDEO] = pButtonContainer->createChild("OgreTray/Group", "VideoOptionsContainer");
  m_pOptionPages[OPTIONS_VIDEO]->setText("Input");
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
}
CMainMenu::~CMainMenu() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CMainMenu::createResources() {
  if (!m_pMMRoot->getChild("Background")) {
    throw Ogre::Exception(0, "asd", __FILE__);
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
    }
    else {
      m_pSaveStatesWindow->setAlpha(max(m_pSaveStatesWindow->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
      m_pSaveStatePreviewWindow->setAlpha(max(m_pSaveStatePreviewWindow->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
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
void CMainMenu::changeState(EMainMenuState eState) {
  if (eState == MMS_OPTIONS && (m_eCurrentState == MMS_GAME_ESCAPE || m_eCurrentState == MMS_START)) {
    m_iTargetState[MMS_OPTIONS][OPTIONS_BACK] = m_eCurrentState;
  }

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
  default:
    break;
  }

  m_bSaveListSelected = false;
  m_eCurrentState = eState;
  m_iSelectedSlot = 0;

  
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
    CGameState::getSingleton().setSaveState(NULL);
    CGameState::getSingleton().changeGameState(CGameState::GS_GAME);
    break;
  case MMS_RESULT_LOAD_GAME:
    CGameState::getSingleton().setSaveState(m_pStateToLoad);
    CGameState::getSingleton().changeGameState(CGameState::GS_GAME);
    break;
  case MMS_RESULT_EXIT:
    CGame::getSingleton().shutDown();
    break;
  case MMS_RESULT_BACK_TO_GAME:
    hide();
    unpause(PAUSE_ALL);
    break;
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

  if (eState == MMS_LOAD_GAME) {
    m_pSaveStatesWindow->setVisible(true);
    m_pSaveStatePreviewWindow->setVisible(true);

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
  else {
    m_pSaveStatesWindow->setVisible(false);
    m_pSaveStatePreviewWindow->setVisible(false);
  }

  if (eState == MMS_GAME_ESCAPE) {
    setPause(PAUSE_ALL);
  }
  if (eState == MMS_START) {
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU);
    unpause(PAUSE_ALL);
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
    if (m_eCurrentState == MMS_LOAD_GAME) {
      m_bSaveListSelected = !m_bSaveListSelected;
      if (m_bSaveListSelected) {
        m_iSelectedSlot = -1;
      }
      else {
        m_iSelectedSlot = LOAD_GAME_BACK;
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
  if (m_iSelectedLoadState < 0 || m_iSelectedLoadState >= static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
    m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/none");
    return;
  }
  m_pStateToLoad = static_cast<const CSaveState*>(m_pSaveStatesWindow->getListboxItemFromIndex(m_iSelectedLoadState)->getUserData());
  m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/" + PropertyHelper<int>::toString(m_pStateToLoad->getActID()) + "-" + PropertyHelper<int>::toString(m_pStateToLoad->getSceneID()));
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
  const CEGUI::MouseEventArgs *mea = dynamic_cast<const CEGUI::MouseEventArgs*>(&args);
  CEGUI::ListboxItem *pItem = m_pSaveStatesWindow->getItemAtPoint(mea->position);
  if (!pItem) {return true;}

  m_iSelectedLoadState = m_pSaveStatesWindow->getItemIndex(pItem);
  selectedSaveStateChanged();
  return true;
}
bool CMainMenu::saveStateListMouseClicked(const CEGUI::EventArgs& args) {
  const CEGUI::MouseEventArgs *mea = dynamic_cast<const CEGUI::MouseEventArgs*>(&args);
  CEGUI::ListboxItem *pItem = m_pSaveStatesWindow->getItemAtPoint(mea->position);
  if (!pItem) {return true;}

  m_iSelectedLoadState = m_pSaveStatesWindow->getItemIndex(pItem);
  selectedSaveStateChanged();
  activateLoadState();
  return true;
}
void CMainMenu::activateLoadState() {
  if (m_iSelectedLoadState >= 0
      && m_iSelectedLoadState < static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
    m_pStateToLoad
      = static_cast<const CSaveState*>(m_pSaveStatesWindow->getListboxItemFromIndex(m_iSelectedLoadState)->getUserData());
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
  m_pOptionPages[OPTIONS_INPUT]->setFont(bigfont);
#ifdef INPUT_TOUCH
  m_pOptionPages[OPTIONS_INPUT]->getChild("ButtonSliderText")->setFont(smallfont);
#endif
  m_pOptionPages[OPTIONS_VIDEO]->setFont(bigfont);
  m_pOptionPages[OPTIONS_VIDEO]->getChild("MenuSizeText")->setFont(smallfont);
}
bool CMainMenu::buttonSizeSliderValueChanged(const EventArgs &args) {
#ifdef INPUT_TOUCH
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  const Slider *pSlider = dynamic_cast<const Slider*>(wndArgs.window);

  m_pOptionPages[OPTIONS_INPUT]->getChild("ButtonSliderText")->setText("Touch button size: " + PropertyHelper<int>::toString(pSlider->getCurrentValue()) + " px");

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
#ifdef INPUT_TOUCH
  Slider *pSlider = dynamic_cast<Slider*>(m_pOptionPages[OPTIONS_INPUT]->getChild("ButtonSizeSlider"));
  pSlider->setMaxValue(min(vSize.d_height / 4.0, vSize.d_width / 8.0));
#endif
}
