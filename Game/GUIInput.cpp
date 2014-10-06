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

#include "GUIInput.hpp"
#include "GameInputManager.hpp"
#include "GameInputCommand.hpp"
#include <OgreLogManager.h>
#include "Weapon.hpp"
#include "Game.hpp"
#include "MessageHandler.hpp"
#include "GUIManager.hpp"

using namespace CEGUI;

CGUIInput::CGUIInput(CEGUI::Window *pGUIRoot)
  : m_pRoot(pGUIRoot),
    m_fTimer(0) {
  CMessageHandler::getSingleton().addInjector(this);

  m_pDirectionButtonContainer = pGUIRoot->createChild("DefaultWindow", "ButtonContainer");
  m_pDirectionButtonContainer->setInheritsAlpha(false);

  m_pControlButtonContainer = pGUIRoot->createChild("DefaultWindow", "ControlContainer");
  m_pControlButtonContainer->setInheritsAlpha(false);

  for (int i = 0; i < BT_COUNT; i++) {
    m_pButtons[i] = createButton(i);
  }


  m_pDirectionButtonContainer->setAlwaysOnTop(true);
  m_pControlButtonContainer->setAlwaysOnTop(true);
  // initial size will be set by options input, the following line would have no effect
  // this->buttonSizeChanged(85);

  hide();
}
CGUIInput::~CGUIInput() {
  CMessageHandler::getSingleton().removeInjector(this);
}
CGUIInput::EButtonTypes CGUIInput::parseButtonType(const std::string &s) {
  if (s == "left") {return BT_LEFT;}
  else if (s == "right") {return BT_RIGHT;}
  else if (s == "enter_link") {return BT_ENTER_LINK;}
  else if (s == "activate") {return BT_ACTIVATE;}
  else if (s == "jump") {return BT_JUMP;}
  else if (s == "attack") {return BT_ATTACK;}

  return BT_COUNT;
}
void CGUIInput::windowResized() {
  buttonSizeChanged(m_fButtonSize);
}
void CGUIInput::buttonSizeChanged(float fSize) {
  m_fButtonSize = fSize;


  // resize direction button container
  m_pDirectionButtonContainer->setPosition(UVector2(UDim(0, 0), UDim(1, -fSize)));
  m_pDirectionButtonContainer->setSize(USize(UDim(0, 2 * fSize), UDim(0, fSize)));

  // resize controll button container
  m_pControlButtonContainer->setPosition(UVector2(UDim(1, -fSize), UDim(1, -4 * fSize)));
  m_pControlButtonContainer->setSize(USize(UDim(0, fSize), UDim(0, 4 * fSize)));

  // resize push buttons
  float fHeight =
    CGUIManager::getSingleton().getRenderer()->getDisplaySize().d_height;
  float fWidth =
    CGUIManager::getSingleton().getRenderer()->getDisplaySize().d_width;

  for (unsigned int i = 0; i < BT_COUNT; i++) {
    m_pButtons[i]->setSize(USize(UDim(0, fSize), UDim(0, fSize)));
    if (i > BT_RIGHT) {
      m_pButtons[i]->setPosition(UVector2(UDim(0, 0), UDim(0, (i - BT_ENTER_LINK) * fSize)));
      m_vButtonOrigins[i].x = fWidth - fSize;
      m_vButtonOrigins[i].y = fHeight - (4 - (i - BT_ENTER_LINK)) * fSize;
    }
    else {
      m_pButtons[i]->setPosition(UVector2(UDim(0, (i == BT_RIGHT) ? fSize : 0), UDim(0, 0)));
      m_vButtonOrigins[i].x = (i == BT_RIGHT) ? fSize : 0;
      m_vButtonOrigins[i].y = fHeight - fSize;
    }
  }
}
CEGUI::Window *CGUIInput::createButton(int bt) {
  Window *pParent = m_pControlButtonContainer;
  if (bt <= BT_RIGHT) {pParent = m_pDirectionButtonContainer;}
  Window *pButton = pParent->createChild("OgreTray/StaticImage", "Button" + PropertyHelper<int>::toString(bt));
  pButton->setSize(USize(UDim(0, 100), UDim(0, 100)));
  pButton->setAlpha(0.7);

  // this is default for control buttons
  pButton->setPosition(UVector2(UDim(0, 0), UDim(0, (bt - BT_ENTER_LINK) * 100)));
  switch (bt) {
  case BT_LEFT:
    pButton->setProperty("Image", "hud_weapons/left_arrow");
    break;
  case BT_RIGHT:
    pButton->setProperty("Image", "hud_weapons/right_arrow");
    break;
  case BT_JUMP:
    pButton->setProperty("Image", "hud_weapons/up_arrow");
    break;
  case BT_ENTER_LINK:
    pButton->setProperty("Image", "hud_weapons/door");
    break;
  case BT_ACTIVATE:
    pButton->setProperty("Image", "hud_weapons/switch");
    break;
  }

  return pButton;
}

void CGUIInput::update(float tpf) {
  if (m_lBlinkingButtons.size() > 0) {
    m_fTimer += tpf;

    float fColValue = (cos(m_fTimer * 6) + 1) * 0.5;
    for (auto pButton : m_lBlinkingButtons) {
      pButton->setProperty("ImageColours", PropertyHelper<Colour>::toString(Colour(1, fColValue, fColValue)));
    }
  }

  updateInput();
}

void CGUIInput::updateInput() {
  for (auto &state : m_bButtonPressed) {
    state = false;
  }

  OIS::Mouse *pMouse = CGame::getSingleton().getInputContext().mMouse;
  if (pMouse) {
    checkForButtonPress(Ogre::Vector2(pMouse->getMouseState().X.abs,
				      pMouse->getMouseState().Y.abs));
  }

  OIS::MultiTouch *pMultiTouch = CGame::getSingleton().getInputContext().mMultiTouch;
  if (pMultiTouch) {
    const std::vector<OIS::MultiTouchState> &mts = pMultiTouch->getMultiTouchStates();
    for (auto &state : mts) {
      if (state.touchType == OIS::MT_Moved || state.touchType == OIS::MT_Pressed) {
	//Ogre::LogManager::getSingleton().logMessage("Pressed buttons: " + Ogre::StringConverter::toString(mts.size()));
	checkForButtonPress(Ogre::Vector2(state.X.abs,
					  state.Y.abs));
      }
    }
  }


  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_LEFT,
					     m_bButtonPressed[BT_LEFT]));
  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_RIGHT,
					     m_bButtonPressed[BT_RIGHT]));
  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_ATTACK,
					     m_bButtonPressed[BT_ATTACK]));
  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_JUMP,
					     m_bButtonPressed[BT_JUMP]));
  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_ACTIVATE,
					     m_bButtonPressed[BT_ACTIVATE]));
  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_ENTER_LINK,
					     m_bButtonPressed[BT_ENTER_LINK]));
}
void CGUIInput::checkForButtonPress(const Ogre::Vector2 &vPos) {
  //Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(vPos));
  int btn = getButtonAtPos(vPos);
  if (btn >= 0 && btn < BT_COUNT) {
    m_bButtonPressed[btn] = true;
  }
}
int CGUIInput::getButtonAtPos(const Ogre::Vector2 &vPos) {
  for (int i = 0; i < BT_COUNT; i++) {
    Ogre::Vector2 posInButton = vPos - m_vButtonOrigins[i];
    if (posInButton.x > 0 && posInButton.x < m_fButtonSize &&
	posInButton.y > 0 && posInButton.y < m_fButtonSize) {
      return i;
      break;
    }
  }
  return BT_COUNT;
}
void CGUIInput::sendMessageToAll(const CMessage &message) {
  switch (message.getType()) {
  case CMessage::MT_TOGGLE_TOUCH_INPUT_BLINK:
    {
      CEGUI::Window *pButton = nullptr;
      EButtonTypes bt = parseButtonType(message.getID());
      if (bt != BT_COUNT) {
	pButton = m_pButtons[bt];
      }
      if (pButton) {
	bool bEnable = message.getBool();
	if (bEnable) {m_lBlinkingButtons.push_back(pButton);}
	else {
	  pButton->setProperty("ImageColours", "FFFFFFFF");
	  m_lBlinkingButtons.remove(pButton);
	}
      }
      if (m_lBlinkingButtons.size() == 0) {
	m_fTimer = 0;
      }
    }
    break;
  case CMessage::MT_MAP_DESTROYED:
    for (auto *pButton : m_pButtons) {
      pButton->setProperty("ImageColours", "FFFFFFFF");
    }
    m_lBlinkingButtons.clear();
    m_fTimer = 0;
    break;
  case CMessage::MT_WEAPON_CHANGED:
    // set the image of the touch button
    m_pButtons[BT_ATTACK]->setProperty("Image", Weapon::getPicture(message.getInt()));
    break;
  default:
    break;
  }
}

void CGUIInput::show() {
  m_pDirectionButtonContainer->setVisible(true);
  m_pControlButtonContainer->setVisible(true);
}
void CGUIInput::hide() {
  m_pDirectionButtonContainer->setVisible(false);
  m_pControlButtonContainer->setVisible(false);
}
