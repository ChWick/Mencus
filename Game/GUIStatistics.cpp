#include "GUIStatistics.hpp"
#include "MainMenu.hpp"
#include "InputDefines.hpp"
#include "GameState.hpp"
#include "SaveStateManager.hpp"

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

  CEGUI::Window *pRetryButton = pButtonContainer->createChild("OgreTray/Button", "Retry");
  pRetryButton->setPosition(UVector2(UDim(0.2, 0), UDim(0.85, 0)));
  pRetryButton->setSize(USize(UDim(0.2, 0), UDim(0.1, 0)));
  pRetryButton->setFont("dejavusans12");
  pRetryButton->setText("Retry");
  m_pButtons[BT_RETRY] = pRetryButton;
  

  CEGUI::Window *pToMenuButton = pButtonContainer->createChild("OgreTray/Button", "ToMenu");
  pToMenuButton->setPosition(UVector2(UDim(0.6, 0), UDim(0.85, 0)));
  pToMenuButton->setSize(USize(UDim(0.2, 0), UDim(0.1, 0)));
  pToMenuButton->setFont("dejavusans12");
  pToMenuButton->setText("To main menu");
  m_pButtons[BT_TO_MENU] = pToMenuButton;

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
}
void CGUIStatistics::activateButton(int iBtn) {
  switch (iBtn) {
  case BT_RETRY:
    CGameState::getSingleton().
      changeGameState(CGameState::GS_GAME, CGameState::getSingleton().getMapInfo());
    break;
  case BT_TO_MENU:
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU);
    break;
  }
}
