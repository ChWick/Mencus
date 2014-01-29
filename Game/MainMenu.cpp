#include "MainMenu.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "SaveStateManager.hpp"

#include <OgreStringConverter.h>
#include <iostream>

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
  ImageManager::getSingleton().loadImageset("save_pictures.imageset");

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

  m_iTargetState[MMS_OPTIONS][OPTIONS_BACK] = MMS_START;
  m_sButtonLabels[MMS_OPTIONS][OPTIONS_BACK] = "Back";

  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_BACK_TO_GAME] = MMS_RESULT_BACK_TO_GAME;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_BACK_TO_GAME] = "Back to game";
  m_iTargetState[MMS_GAME_ESCAPE][GAMES_ESCAPE_EXIT_GAME] = MMS_START;
  m_sButtonLabels[MMS_GAME_ESCAPE][GAMES_ESCAPE_EXIT_GAME] = "Exit game";

  // create cegui windows/buttons
  m_pMMRoot = pGUIRoot->createChild("OgreTray/StaticImage", "MainMenuRoot");
  m_pMMRoot->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  m_pMMRoot->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
  m_pMMRoot->setInheritsAlpha(false);
  m_pMMRoot->setAlpha(1);
  m_pMMRoot->setProperty("Image", "main_menu_background/full_image");
  m_pMMRoot->setProperty("FrameEnabled", "False");
  m_pMMRoot->setProperty("BackgroundEnabled", "True");

  for (int i = 0; i < NUM_SLOTS; i++) {
    m_vSlots[i] = m_pMMRoot->createChild("OgreTray/Button", "Slot" + CEGUI::PropertyHelper<int>::toString(i));
    m_vSlots[i]->setSize(USize(UDim(0.2f, 0), UDim(0.07f, 0)));
    m_vSlots[i]->setPosition(UVector2(UDim(0.2f, 0), UDim(0.5f + 0.10f * i, 0)));
    m_vSlots[i]->setAlpha(BUTTON_MIN_ALPHA);
    m_vSlots[i]->setFont("dejavusans12");
  }

  m_pSaveStatesWindow = dynamic_cast<CEGUI::Listbox*>(m_pMMRoot->createChild("OgreTray/Listbox", "SaveStatesBox"));
  m_pSaveStatesWindow->setPosition(UVector2(UDim(0.2f, 0), UDim(0.5f, 0)));
  m_pSaveStatesWindow->setSize(USize(UDim(0.3f, 0), UDim(0.07f + 2 * 0.1f, 0)));
  m_pSaveStatesWindow->setFont("dejavusans8");

  m_pSaveStatePreviewWindow = m_pMMRoot->createChild("OgreTray/StaticImage", "PreviewPricture");
  m_pSaveStatePreviewWindow->setPosition(UVector2(UDim(0.55f, 0), UDim(0.5f, 0)));
  m_pSaveStatePreviewWindow->setSize(USize(UDim(0.2f, 0), UDim(0.2f, 0)));
  m_pSaveStatePreviewWindow->setProperty("Image", "save_pictures/none");
}
CMainMenu::~CMainMenu() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CMainMenu::update(Ogre::Real tpf) {
  if (!m_pMMRoot->isVisible()) { return; }

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

    for (int i = 0; i < static_cast<int>(m_pSaveStatesWindow->getItemCount()); ++i) {
      if (m_iSelectedLoadState == i) {
        dynamic_cast<ListboxTextItem*>(m_pSaveStatesWindow->getListboxItemFromIndex(i))->setTextColours(Colour(1, 1, 1));
      }
      else {
        dynamic_cast<ListboxTextItem*>(m_pSaveStatesWindow->getListboxItemFromIndex(i))->setTextColours(Colour(0, 0, 0));
      }
    }
  }
}
void CMainMenu::changeState(EMainMenuState eState) {
  switch (m_eCurrentState) {
  case MMS_RESULT_NEW_GAME:
    break;
  default:
    break;
  }

  m_bSaveListSelected = false;
  m_eCurrentState = eState;
  m_iSelectedSlot = 0;

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
        m_vSlots[i]->setAlpha(BUTTON_MIN_ALPHA);
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
      ListboxTextItem *pItem = new ListboxTextItem("Act " + PropertyHelper<int>::toString(state.getActID()) +
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
      if (m_iSelectedLoadState >= 0 && m_iSelectedLoadState < static_cast<int>(m_pSaveStatesWindow->getItemCount())) {
        m_pStateToLoad = static_cast<const CSaveState*>(m_pSaveStatesWindow->getListboxItemFromIndex(m_iSelectedLoadState)->getUserData());
        changeState(MMS_RESULT_LOAD_GAME);
      }
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
}
