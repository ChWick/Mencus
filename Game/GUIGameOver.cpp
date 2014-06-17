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

#include "GUIGameOver.hpp"
#include "GameState.hpp"
#include "SaveStateManager.hpp"


template<> CGUIGameOver *Ogre::Singleton<CGUIGameOver>::msSingleton = 0;

CGUIGameOver *CGUIGameOver::getSingletonPtr() {
  return msSingleton;
}
CGUIGameOver &CGUIGameOver::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CGUIGameOver::CGUIGameOver(CEGUI::Window *pGUIRoot)
  : m_iSelectedText(0) {
  using namespace CEGUI;

  ImageManager::getSingleton().loadImageset("game_over.imageset");

  m_pRoot = pGUIRoot->createChild("OgreTray/StaticImage", "Instructions/Root");
  m_pRoot->setInheritsAlpha(false);
  m_pRoot->setAlpha(1);
  m_pRoot->setSize(USize(UDim(1, 0), UDim(1, 0)));
  m_pRoot->setProperty("Image", "game_over/full");
  m_pRoot->setProperty("FrameEnabled","False");
  m_pRoot->setProperty("BackgroundEnabled","False");
  m_pRoot->moveToBack();

  m_pRetryText = m_pRoot->createChild("OgreTray/StaticText", "Retry");
  m_pRetryText->setSize(USize(UDim(0.2, 0), UDim(0.1,0)));
  m_pRetryText->setPosition(UVector2(UDim(0.40, 0), UDim(0.85, 0)));
  m_pRetryText->setText("Retry level");
  m_pRetryText->setProperty("HorzFormatting", "HorzCentred");
  m_pRetryText->setProperty("FrameEnabled","False");
  m_pRetryText->setProperty("BackgroundEnabled","False");
  m_pRetryText->setFont("dejavusans12");
  m_pRetryText->setProperty("TextColours","FFFFFFFF");
  m_pRetryText->setAlpha(0.3f);

  m_pExitText = m_pRoot->createChild("OgreTray/StaticText", "Exit");
  m_pExitText->setSize(USize(UDim(0.2, 0), UDim(0.1,0)));
  m_pExitText->setPosition(UVector2(UDim(0.40, 0), UDim(0.9, 0)));
  m_pExitText->setText("To main menu");
  m_pExitText->setProperty("HorzFormatting", "HorzCentred");
  m_pExitText->setProperty("FrameEnabled","False");
  m_pExitText->setProperty("BackgroundEnabled","False");
  m_pExitText->setFont("dejavusans12");
  m_pExitText->setProperty("TextColours","FFFFFFFF");
  m_pExitText->setAlpha(0.3f);

  CInputListenerManager::getSingleton().addInputListener(this);

  hide();
}
CGUIGameOver::~CGUIGameOver() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CGUIGameOver::update(Ogre::Real tpf) {
  if (m_iSelectedText == 0) {
    m_pRetryText->setAlpha(std::min<float>(m_pRetryText->getAlpha() + tpf, 1.0f));
    m_pExitText->setAlpha(std::max<float>(m_pExitText->getAlpha() - tpf, 0.3f));
  }
  else {
    m_pExitText->setAlpha(std::min<float>(m_pExitText->getAlpha() + tpf, 1.0f));
    m_pRetryText->setAlpha(std::max<float>(m_pRetryText->getAlpha() - tpf, 0.3f));
  }

  hide();
}
bool CGUIGameOver::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_UP) {
    m_iSelectedText = std::max<int>(m_iSelectedText - 1, 0);
  }
  else if (arg.key == OIS::KC_DOWN) {
    m_iSelectedText = std::min<int>(m_iSelectedText + 1, 1);
  }
  else if (arg.key == OIS::KC_RETURN) {
    if (m_iSelectedText == 0) {
      CGameState::getSingleton().
	changeGameState(CGameState::GS_GAME, CSaveStateManager::getSingleton().getLastSaveState());
    }
    else {
      CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU);
    }
  }
  return true;
}
