#include "GUIStatistics.hpp"
#include "MainMenu.hpp"
#include "InputDefines.hpp"
#include "GameState.hpp"
#include "SaveStateManager.hpp"
#include "Statistics.hpp"
#include "LevelState.hpp"
#include <OgreStringConverter.h>

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
  pFinishedText->setFont("dejavusans12");
  pFinishedText->setText("Mission accomplished");
  pFinishedText->setPosition(UVector2(UDim(0.05,0), UDim(0.05,0)));
  pFinishedText->setSize(USize(UDim(0.9, 0), UDim(0.1, 0))); 

  CEGUI::Window *pStatisticsGroup = pButtonContainer->createChild("OgreTray/Group", "statisticsgroup");
  pStatisticsGroup->setFont("dejavusans12");
  pStatisticsGroup->setText("Statistics");
  pStatisticsGroup->setPosition(UVector2(UDim(0.2, 0), UDim(0.2, 0)));
  pStatisticsGroup->setSize(USize(UDim(0.6, 0), UDim(0.5, 0)));

  CEGUI::Window *pRetryButton = pButtonContainer->createChild("OgreTray/Button", "Retry");
  pRetryButton->setPosition(UVector2(UDim(0.2, 0), UDim(0.85, 0)));
  pRetryButton->setSize(USize(UDim(0.2, 0), UDim(0.1, 0)));
  pRetryButton->setFont("dejavusans12");
  pRetryButton->setText("Retry");
  m_pButtons[BT_RETRY] = pRetryButton;
  pRetryButton->subscribeEvent(
			       CEGUI::PushButton::EventClicked,
			       CEGUI::Event::Subscriber(&CGUIStatistics::onRetryClicked, this));
  

  CEGUI::Window *pToMenuButton = pButtonContainer->createChild("OgreTray/Button", "ToMenu");
  pToMenuButton->setPosition(UVector2(UDim(0.6, 0), UDim(0.85, 0)));
  pToMenuButton->setSize(USize(UDim(0.2, 0), UDim(0.1, 0)));
  pToMenuButton->setFont("dejavusans12");
  pToMenuButton->setText("To level selection");
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
    m_iSelectedSlot = std::max<int>(m_iSelectedSlot - 1, 0);
    break;
  case OIS::KC_RIGHT:
    m_iSelectedSlot = std::min<int>(m_iSelectedSlot + 1, BT_COUNT - 1);
    break;
  case OIS::KC_RETURN:
    activateButton(m_iSelectedSlot);
    break;
  default:
    break;
  }
  return true;
}
void CGUIStatistics::activateButton(int iBtn) {
  switch (iBtn) {
  case BT_RETRY:
    CGameState::getSingleton().
      changeGameState(CGameState::GS_GAME, CGameState::getSingleton().getMapInfo());
    break;
  case BT_TO_MENU:
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU, MainMenu::MMS_USER_GAME);
    break;
  }
}
void CGUIStatistics::resize(const CEGUI::String &smallfont, const CEGUI::String &bigfont) {
  for (int i = 0; i < BT_COUNT; i++) {
    m_pButtons[i]->setFont(smallfont);
  }
  Window *pButtonContainer = m_pStatisticsRoot->getChild("ButtonContainer");
  pButtonContainer->getChild("text")->setFont(smallfont);
  pButtonContainer->getChild("statisticsgroup")->setFont(smallfont);

  for (int i = 0; i < L_COUNT; i++) {
    pButtonContainer->getChild("statisticsgroup")->getChild("Label" + PropertyHelper<int>::toString(i))->setFont(smallfont);
    pButtonContainer->getChild("statisticsgroup")->getChild("Data" + PropertyHelper<int>::toString(i))->setFont(smallfont);

  }
}
void CGUIStatistics::showStatistics(const SStatistics &stats) {
  CLevelState::add(stats);

  Window *pButtonContainer = m_pStatisticsRoot->getChild("ButtonContainer");
  Window *group = pButtonContainer->getChild("statisticsgroup");
  Window *pTopText = pButtonContainer->getChild("text");

  int iHours = static_cast<int>(stats.fTime / 3600);
  int iMinutes = static_cast<int>((stats.fTime - iHours * 3600) / 60);
  int iSeconds = static_cast<int>(stats.fTime - iHours * 3600 - iMinutes * 60);

  switch (stats.eMissionState) {
  case MS_ACCOMPLISHED:
    pTopText->setText("Mission accomplished");
    pTopText->setProperty("CaptionColour", "FF66FF66");
    break;
  case MS_FAILED:
    pTopText->setText("Mission failed");
    pTopText->setProperty("CaptionColour", "FF992222");
    break;
  default:
    pTopText->setText("Unknown mission state");
    break;
  }

  group->getChild("Data" + PropertyHelper<int>::toString(L_TIME))
    ->setText((Ogre::StringConverter::toString(iHours, 1) + ":"
	       + Ogre::StringConverter::toString(iMinutes, 2, '0') + ":"
	       + Ogre::StringConverter::toString(iSeconds, 2, '0')).c_str());

  group->getChild("Data" + PropertyHelper<int>::toString(L_HITPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fHitpoints));
  
  group->getChild("Data" + PropertyHelper<int>::toString(L_MANAPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fManapoints));

  group->getChild("Data" + PropertyHelper<int>::toString(L_LOST_HITPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fLostHitpoints));

  group->getChild("Data" + PropertyHelper<int>::toString(L_USED_MANAPOINTS))
    ->setText(PropertyHelper<int>::toString(stats.fUsedManapoints));
}
void CGUIStatistics::createLabel(int iLabel, CEGUI::Window *pParent, bool bData) {
  Window *pLabel = pParent->createChild("OgreTray/Label", ((!bData) ? "Label" : "Data") + PropertyHelper<int>::toString(iLabel));
  //pLabel->setProperty("FrameEnabled", "False");
  //pLabel->setProperty("BackgroundEnabled", "False");
  pLabel->setProperty("HorzFormatting", "LeftAligned");
  pLabel->setPosition(UVector2(UDim(bData ? 0.4 : 0.05, 0), UDim(0.05 + iLabel * 0.1, 0)));
  pLabel->setSize(USize(UDim(bData ? 0.55 : 0.4, 0), UDim(0.09, 0)));

  if (bData) {
    pLabel->setText("to be filled");
  }
  else {
    switch (iLabel) {
    case L_TIME:
      pLabel->setText("Time");
      break;
    case L_HITPOINTS:
      pLabel->setText("Hitpoints");
      break;
    case L_MANAPOINTS:
      pLabel->setText("Manapoints");
      break;
    case L_LOST_HITPOINTS:
      pLabel->setText("Lost hitpoints");
      break;
    case L_USED_MANAPOINTS:
      pLabel->setText("Used manapoints");
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
