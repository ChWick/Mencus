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

#include "GUIStatistics.hpp"
#include "MainMenu.hpp"
#include "InputDefines.hpp"
#include "GameState.hpp"
#include "SaveStateManager.hpp"
#include "Statistics.hpp"
#include <OgreStringConverter.h>
#include "XMLResources/Manager.hpp"

using namespace CEGUI;

template<> CGUIStatistics *Ogre::Singleton<CGUIStatistics>::msSingleton = 0;

CGUIStatistics *CGUIStatistics::getSingletonPtr() {
  return msSingleton;
}
CGUIStatistics &CGUIStatistics::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CGUIStatistics::CGUIStatistics(Window *pRoot)
  : m_iSelectedSlot(0) {
  CInputListenerManager::getSingleton().addInputListener(this);
  // create cegui windows/buttons
  Window *pStatisticsRoot = pRoot->createChild("DefaultWindow", "GUIStatisticsRoot");
  pStatisticsRoot->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  pStatisticsRoot->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
  pStatisticsRoot->setInheritsAlpha(false);
  pStatisticsRoot->setAlpha(1);
  pStatisticsRoot->setRiseOnClickEnabled(false);
  pStatisticsRoot->moveToBack();
  m_pStatisticsRoot = pStatisticsRoot;

  CEGUI::Window *pBackground = pStatisticsRoot->
    createChild("OgreTray/StaticImage", "Background");
  pBackground->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  pBackground->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
  pBackground->setInheritsAlpha(false);
  pBackground->setAlpha(1);
  pBackground->setProperty("Image", "main_menu_background/full");
  pBackground->setProperty("FrameEnabled", "False");
  pBackground->setProperty("BackgroundEnabled", "True");
  pBackground->setRiseOnClickEnabled(false);

  CEGUI::Window *pButtonContainer = pStatisticsRoot->createChild("DefaultWindow", "ButtonContainer");
  pButtonContainer->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pButtonContainer->setSize(USize(UDim(1, 0), UDim(1, 0)));

  CEGUI::Window *pFinishedText = pButtonContainer->createChild("OgreTray/Titlebar", "text");
  pFinishedText->setFont("dejavusans20");
  pFinishedText->setText("unset");
  pFinishedText->setPosition(UVector2(UDim(0.05,0), UDim(0.05,0)));
  pFinishedText->setSize(USize(UDim(0.9, 0), UDim(0.1, 0)));

  CEGUI::Window *pStatisticsGroup = pButtonContainer->createChild("OgreTray/Group", "statisticsgroup");
  pStatisticsGroup->setFont("dejavusans16");
  pStatisticsGroup->setText(XMLResources::GLOBAL.getCEGUIString("Statistics"));
  pStatisticsGroup->setPosition(UVector2(UDim(0.1, 0), UDim(0.2, 0)));
  pStatisticsGroup->setSize(USize(UDim(0.8, 0), UDim(0.5, 0)));

  CEGUI::Window *pRetryButton = pButtonContainer->createChild("OgreTray/Button", "Retry");
  pRetryButton->setPosition(UVector2(UDim(0.1, 0), UDim(0.8, 0)));
  pRetryButton->setSize(USize(UDim(0.3, 0), UDim(0.15, 0)));
  pRetryButton->setFont("dejavusans16");
  pRetryButton->setText(XMLResources::GLOBAL.getCEGUIString("Retry"));
  m_pButtons[BT_RETRY] = pRetryButton;
  pRetryButton->subscribeEvent(
			       CEGUI::PushButton::EventClicked,
			       CEGUI::Event::Subscriber(&CGUIStatistics::onRetryClicked, this));


  CEGUI::Window *pToMenuButton = pButtonContainer->createChild("OgreTray/Button", "ToMenu");
  pToMenuButton->setPosition(UVector2(UDim(0.6, 0), UDim(0.8, 0)));
  pToMenuButton->setSize(USize(UDim(0.3, 0), UDim(0.15, 0)));
  pToMenuButton->setFont("dejavusans16");
  pToMenuButton->setText(XMLResources::GLOBAL.getCEGUIString("To level selection"));
  pToMenuButton->subscribeEvent(
				CEGUI::PushButton::EventClicked,
				CEGUI::Event::Subscriber(&CGUIStatistics::onToMenuClicked, this));
  m_pButtons[BT_TO_MENU] = pToMenuButton;

  for (int i = 0; i < L_COUNT; i++) {
    createLabel(i, pStatisticsGroup, false);
    createLabel(i, pStatisticsGroup, true);
  }

  hide();
}
CGUIStatistics::~CGUIStatistics() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CGUIStatistics::update(Ogre::Real tpf) {
#ifdef INPUT_KEYBOARD_ONLY
  for (int i = 0; i < BT_COUNT; i++) {
    if (i == m_iSelectedSlot) {
      m_pButtons[i]->setAlpha(std::min<Ogre::Real>(m_pButtons[i]->getAlpha() + tpf * BUTTON_ALPHA_CHANGE_PER_SEC, 1.0f));
    }
    else {
      m_pButtons[i]->setAlpha(std::max<Ogre::Real>(m_pButtons[i]->getAlpha() - tpf * BUTTON_ALPHA_CHANGE_PER_SEC, BUTTON_MIN_ALPHA));
    }
  }
#endif
}
bool CGUIStatistics::keyPressed(const OIS::KeyEvent &arg) {
  switch (arg.key) {
  case OIS::KC_LEFT:
    changeSelectedSlot(m_iSelectedSlot - 1);
    break;
  case OIS::KC_RIGHT:
    changeSelectedSlot(m_iSelectedSlot + 1);
    break;
  case OIS::KC_RETURN:
    activateButton(m_iSelectedSlot);
    break;
  default:
    break;
  }

  return true;
}

void CGUIStatistics::changeSelectedSlot(int i) {
  m_iSelectedSlot = std::min<int>(std::max<int>(i, 0), BT_COUNT - 1);
  
  Vector2f pos = m_pButtons[m_iSelectedSlot]->getInnerRectClipper().getPosition();
  Sizef size = m_pButtons[m_iSelectedSlot]->getInnerRectClipper().getSize();
  // next line is to be shure that
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(pos.d_x + size.d_width / 2, pos.d_y + size.d_height / 2);
}
void CGUIStatistics::activateButton(int iBtn) {
  switch (iBtn) {
  case BT_RETRY:
    CGameState::getSingleton().
      changeGameState(CGameState::GS_GAME, CGameState::getSingleton().getMapPack());
    break;
  case BT_TO_MENU:
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU, MainMenu::MMS_USER_GAME);
    break;
  }
}
void CGUIStatistics::resize(const CEGUI::String &smallfont, const CEGUI::String &bigfont) {
  /*for (int i = 0; i < BT_COUNT; i++) {
    m_pButtons[i]->setFont(bigfont);
  }
  Window *pButtonContainer = m_pStatisticsRoot->getChild("ButtonContainer");
  pButtonContainer->getChild("text")->setFont(bigfont);
  pButtonContainer->getChild("statisticsgroup")->setFont(bigfont);

  for (int i = 0; i < L_COUNT; i++) {
    pButtonContainer->getChild("statisticsgroup")->getChild("Label" + PropertyHelper<int>::toString(i))->setFont(smallfont);
    pButtonContainer->getChild("statisticsgroup")->getChild("Data" + PropertyHelper<int>::toString(i))->setFont(smallfont);

    }*/
}
void CGUIStatistics::showStatistics(const SStatistics &stats) {
  Window *pButtonContainer = m_pStatisticsRoot->getChild("ButtonContainer");
  Window *group = pButtonContainer->getChild("statisticsgroup");
  Window *pTopText = pButtonContainer->getChild("text");

  int iHours = static_cast<int>(stats.fTime / 3600);
  int iMinutes = static_cast<int>((stats.fTime - iHours * 3600) / 60);
  int iSeconds = static_cast<int>(stats.fTime - iHours * 3600 - iMinutes * 60);

  switch (stats.eMissionState) {
  case MissionState::MS_ACCOMPLISHED:
    pTopText->setText(XMLResources::GLOBAL.getCEGUIString("Mission accomplished"));
    pTopText->setProperty("CaptionColour", "FF66FF66");
    changeSelectedSlot(BT_TO_MENU);
    break;
  case MissionState::MS_FAILED:
    pTopText->setText(XMLResources::GLOBAL.getCEGUIString("Mission failed"));
    pTopText->setProperty("CaptionColour", "FF992222");
    changeSelectedSlot(BT_RETRY);
    break;
  default:
    pTopText->setText("Unknown mission state");
    changeSelectedSlot(0);
    break;
  }

  group->getChild("Data" + PropertyHelper<int>::toString(L_TIME))
    ->setText((Ogre::StringConverter::toString(iHours, 1) + ":"
	       + Ogre::StringConverter::toString(iMinutes, 2, '0') + ":"
	       + Ogre::StringConverter::toString(iSeconds, 2, '0')).c_str());

  // round all values
  group->getChild("Data" + PropertyHelper<int>::toString(L_HITPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fHitpoints + 0.5));

  group->getChild("Data" + PropertyHelper<int>::toString(L_MANAPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fManapoints + 0.5));

  group->getChild("Data" + PropertyHelper<int>::toString(L_LOST_HITPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fLostHitpoints + 0.5));

  group->getChild("Data" + PropertyHelper<int>::toString(L_USED_MANAPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fUsedManapoints + 0.5));

  group->getChild("Data" + PropertyHelper<int>::toString(L_KILLED_ENEMIES))
    ->setText(PropertyHelper<int>::toString(stats.iKilledEnemies));
}
void CGUIStatistics::createLabel(int iLabel, CEGUI::Window *pParent, bool bData) {
  Window *pLabel = pParent->createChild("OgreTray/Label", ((!bData) ? "Label" : "Data") + PropertyHelper<int>::toString(iLabel));
  //pLabel->setProperty("FrameEnabled", "False");
  //pLabel->setProperty("BackgroundEnabled", "False");
  pLabel->setProperty("HorzFormatting", "LeftAligned");
  pLabel->setPosition(UVector2(UDim(bData ? 0.6 : 0.05, 0), UDim(0.05 + iLabel * 0.1, 0)));
  pLabel->setSize(USize(UDim(bData ? 0.35 : 0.6, 0), UDim(0.09, 0)));

  if (bData) {
    pLabel->setText("to be filled");
  }
  else {
    switch (iLabel) {
    case L_TIME:
      pLabel->setText(XMLResources::GLOBAL.getCEGUIString("Time"));
      break;
    case L_HITPOINTS:
      pLabel->setText(XMLResources::GLOBAL.getCEGUIString("Hitpoints"));
      break;
    case L_MANAPOINTS:
      pLabel->setText(XMLResources::GLOBAL.getCEGUIString("Manapoints"));
      break;
    case L_LOST_HITPOINTS:
      pLabel->setText(XMLResources::GLOBAL.getCEGUIString("Lost hitpoints"));
      break;
    case L_USED_MANAPOINTS:
      pLabel->setText(XMLResources::GLOBAL.getCEGUIString("Used manapoints"));
      break;
    case L_KILLED_ENEMIES:
      pLabel->setText(XMLResources::GLOBAL.getCEGUIString("Killed enemies"));
      break;
    default:
      pLabel->setText("unset");
      break;
    }
  }
}
bool CGUIStatistics::onRetryClicked(const CEGUI::EventArgs&) {
  activateButton(BT_RETRY);
  return true;
}
bool CGUIStatistics::onToMenuClicked(const CEGUI::EventArgs&) {
  activateButton(BT_TO_MENU);
  return true;
}
void CGUIStatistics::show() {
  m_pStatisticsRoot->setVisible(true);
  setInputListenerEnabled(true);
}
void CGUIStatistics::hide() {
  m_pStatisticsRoot->setVisible(false);
  setInputListenerEnabled(false);
}
