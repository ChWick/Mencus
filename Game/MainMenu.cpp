#include "MainMenu.hpp"
#include "Game.hpp"

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
  : m_vSlots(NUM_SLOTS) {
  CInputListenerManager::getSingleton().addInputListener(this);

  for (int i = 0; i < MMS_COUNT; i++) {
    for (int j = 0; j < NUM_SLOTS; j++) {
      m_iTargetState[i][j] = MMS_STATE_NONE;
      m_sButtonLabels[i][j] = "Undefined";
    }
  }
  // setup slots for the pages
  m_iTargetState[MMS_START][START_START_GAME] = MMS_GAME;
  m_iTargetState[MMS_START][START_OPTIONS] = MMS_OPTIONS;
  m_iTargetState[MMS_START][START_EXIT] = MMS_EXIT;
  m_sButtonLabels[MMS_START][START_START_GAME] = "Start game";
  m_sButtonLabels[MMS_START][START_OPTIONS] = "Options";
  m_sButtonLabels[MMS_START][START_EXIT] = "Exit";

  m_iTargetState[MMS_GAME][GAME_NEW_GAME] = MMS_NEW_GAME;
  m_iTargetState[MMS_GAME][GAME_LOAD_GAME] = MMS_LOAD_GAME;
  m_iTargetState[MMS_GAME][GAME_BACK] = MMS_START;
  m_sButtonLabels[MMS_GAME][GAME_NEW_GAME] = "New game";
  m_sButtonLabels[MMS_GAME][GAME_LOAD_GAME] = "Load game";
  m_sButtonLabels[MMS_GAME][GAME_BACK] = "Back";

  // create cegui windows/buttons
  m_pMMRoot = pGUIRoot->createChild("DefaultWindow", "MainMenuRoot");
  m_pMMRoot->setInheritsAlpha(false);
  m_pMMRoot->setAlpha(1);

  for (int i = 0; i < NUM_SLOTS; i++) {
    m_vSlots[i] = m_pMMRoot->createChild("OgreTray/Button", "Slot" + CEGUI::PropertyHelper<int>::toString(i));
    m_vSlots[i]->setSize(USize(UDim(0.2f, 0), UDim(0.07f, 0)));
    m_vSlots[i]->setPosition(UVector2(UDim(0.2f, 0), UDim(0.5f + 0.10f * i, 0)));
    m_vSlots[i]->setAlpha(BUTTON_MIN_ALPHA);
  }

  changeState(MMS_START);
}
CMainMenu::~CMainMenu() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CMainMenu::update(Ogre::Real tpf) {
  for (int i = 0; i < NUM_SLOTS; i++) {
    if (i == m_iSelectedSlot) {
      m_vSlots[i]->setAlpha(min(m_vSlots[i]->getAlpha() + tpf * BUTTON_ALPHA_CHANGE_PER_SEC, 1.0f));
    }
    else {
      m_vSlots[i]->setAlpha(max(m_vSlots[i]->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
    }
  }
}
void CMainMenu::changeState(EMainMenuState eState) {
  m_eCurrentState = eState;
  m_iSelectedSlot = 0;

  switch (eState) {
  case MMS_NEW_GAME:
    break;
  case MMS_LOAD_GAME:
    break;
  case MMS_EXIT:
    CGame::getSingleton().shutDown();
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
    break;
  }
}
bool CMainMenu::keyPressed(const OIS::KeyEvent &arg) {
  switch (arg.key) {
  case OIS::KC_DOWN:
    ++m_iSelectedSlot;
    while (m_iSelectedSlot >= NUM_SLOTS || m_iTargetState[m_eCurrentState][m_iSelectedSlot] == MMS_STATE_NONE) {
      if (m_iSelectedSlot >= NUM_SLOTS) {
        m_iSelectedSlot = 0;
      }
      else {
        ++m_iSelectedSlot;
      }
    }
    break;
  case OIS::KC_UP:
    --m_iSelectedSlot;
    while (m_iSelectedSlot < 0 || m_iTargetState[m_eCurrentState][m_iSelectedSlot] == MMS_STATE_NONE) {
      if (m_iSelectedSlot < 0) {
        m_iSelectedSlot = NUM_SLOTS - 1;
      }
      else {
        --m_iSelectedSlot;
      }
    }
    break;
  case OIS::KC_RETURN:
    changeState(m_iTargetState[m_eCurrentState][m_iSelectedSlot]);
    break;
  }
  return true;
}