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

#include "GameInputManager.hpp"
#include "GameInputCommand.hpp"
#include "GameInputListener.hpp"
#include "InputListener.hpp"

template<> CGameInputManager* Ogre::Singleton<CGameInputManager>::msSingleton = 0;

CGameInputManager* CGameInputManager::getSingletonPtr(void) {
  return msSingleton;
}
CGameInputManager& CGameInputManager::getSingleton(void) {
  assert( msSingleton );  return ( *msSingleton );
}

CGameInputManager::CGameInputManager() {
  CInputListenerManager::getSingleton().addInputListener(this);
  for (auto &state : m_InputStates) {
    state = GIS_NONE;
  }
}
CGameInputManager::~CGameInputManager() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
bool CGameInputManager::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_LEFT) {
    injectCommand(CGameInputCommand(GIC_LEFT, 1));
  }
  else if (arg.key == OIS::KC_RIGHT) {
    injectCommand(CGameInputCommand(GIC_RIGHT, 1));
  }
  else if (arg.key == OIS::KC_UP) {
    injectCommand(CGameInputCommand(GIC_JUMP, 1));
  }
  else if (arg.key == OIS::KC_DOWN) {
    injectCommand(CGameInputCommand(GIC_ENTER_LINK, 1));
  }
  else if (arg.key == OIS::KC_SPACE) {
    injectCommand(CGameInputCommand(GIC_ATTACK, 1));
  }
  else if (arg.key == OIS::KC_RETURN) {
    injectCommand(CGameInputCommand(GIC_ACTIVATE, 1));
  }
  else if (arg.key == OIS::KC_N) {
    injectCommand(CGameInputCommand(GIC_USE_HEALTH_POTION, 1));
  }
  else if (arg.key == OIS::KC_M) {
    injectCommand(CGameInputCommand(GIC_USE_MANA_POTION, 1));
  }

  return true;
}
bool CGameInputManager::keyReleased( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_LEFT) {
    injectCommand(CGameInputCommand(GIC_LEFT, 0));
  }
  else if (arg.key == OIS::KC_RIGHT) {
    injectCommand(CGameInputCommand(GIC_RIGHT, 0));
  }
  else if (arg.key == OIS::KC_UP) {
    injectCommand(CGameInputCommand(GIC_JUMP, 0));
  }
  else if (arg.key == OIS::KC_DOWN) {
    injectCommand(CGameInputCommand(GIC_ENTER_LINK, 0));
  }
  else if (arg.key == OIS::KC_SPACE) {
    injectCommand(CGameInputCommand(GIC_ATTACK, 0));
  }
  else if (arg.key == OIS::KC_RETURN) {
    injectCommand(CGameInputCommand(GIC_ACTIVATE, 0));
  }

  return true;
}
bool CGameInputManager::mouseMoved( const OIS::MouseEvent &arg ) {return true;}
bool CGameInputManager::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {return true;}
bool CGameInputManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {return true;}
bool CGameInputManager::touchMoved(const OIS::MultiTouchEvent& evt) {return true;}
bool CGameInputManager::touchPressed(const OIS::MultiTouchEvent& evt) {return true;}
bool CGameInputManager::touchReleased(const OIS::MultiTouchEvent& evt) {return true;}
bool CGameInputManager::touchCancelled(const OIS::MultiTouchEvent& evt) {return true;}
void CGameInputManager::injectCommand( const CGameInputCommand &cmd) {
  if (cmd.getState() == GIS_CLICKED) {
    sendCommandToListeners(CGameInputCommand(cmd));
    return;
  }
  if (cmd.getFloatValue() > 0.5) {
    switch (m_InputStates[cmd.getType()]) {
    case GIS_RELEASED:
    case GIS_NONE:
      sendCommandToListeners(CGameInputCommand(cmd, GIS_PRESSED));
      m_InputStates[cmd.getType()] = GIS_PRESSED;
      break;
    case GIS_HOLD:
    case GIS_PRESSED:
      sendCommandToListeners(CGameInputCommand(cmd, GIS_HOLD));
      m_InputStates[cmd.getType()] = GIS_HOLD;
      break;
    }
  }
  else {
    switch (m_InputStates[cmd.getType()]) {
    case GIS_PRESSED:
    case GIS_HOLD:
      sendCommandToListeners(CGameInputCommand(cmd, GIS_RELEASED));
      m_InputStates[cmd.getType()] = GIS_RELEASED;
      break;
    case GIS_RELEASED:
    case GIS_NONE:
      sendCommandToListeners(CGameInputCommand(cmd, GIS_NONE));
      m_InputStates[cmd.getType()] = GIS_NONE;
      break;
    }
  }
}
void CGameInputManager::sendCommandToListeners( const CGameInputCommand &cmd ) {
  for (auto pListener : m_lListenerList) {
    if (pListener->isGameInputListenerEnabled()) {
      pListener->receiveInputCommand(cmd);
    }
  }
}
