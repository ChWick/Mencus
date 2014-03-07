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
}
CGameInputManager::~CGameInputManager() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
bool CGameInputManager::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_LEFT) {
    sendCommandToListeners(CGameInputCommand(GIC_LEFT, 1));
  }
  else if (arg.key == OIS::KC_RIGHT) {
    sendCommandToListeners(CGameInputCommand(GIC_RIGHT, 1));
  }
  else if (arg.key == OIS::KC_UP) {
    sendCommandToListeners(CGameInputCommand(GIC_JUMP, 1));
  }
  else if (arg.key == OIS::KC_DOWN) {
    sendCommandToListeners(CGameInputCommand(GIC_ENTER_LINK, 1));
  }
  else if (arg.key == OIS::KC_SPACE) {
    sendCommandToListeners(CGameInputCommand(GIC_ATTACK, 1));
  }
  else if (arg.key == OIS::KC_RETURN) {
    sendCommandToListeners(CGameInputCommand(GIC_ACTIVATE, 1));
  }

  return true;
}
bool CGameInputManager::keyReleased( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_LEFT) {
    sendCommandToListeners(CGameInputCommand(GIC_LEFT, 0));
  }
  else if (arg.key == OIS::KC_RIGHT) {
    sendCommandToListeners(CGameInputCommand(GIC_RIGHT, 0));
  }
  else if (arg.key == OIS::KC_UP) {
    sendCommandToListeners(CGameInputCommand(GIC_JUMP, 0));
  }
  else if (arg.key == OIS::KC_DOWN) {
    sendCommandToListeners(CGameInputCommand(GIC_ENTER_LINK, 0));
  }
  else if (arg.key == OIS::KC_SPACE) {
    sendCommandToListeners(CGameInputCommand(GIC_ATTACK, 0));
  }
  else if (arg.key == OIS::KC_RETURN) {
    sendCommandToListeners(CGameInputCommand(GIC_ACTIVATE, 0));
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
void CGameInputManager::sendCommandToListeners( const CGameInputCommand &cmd ) {
  for (auto pListener : m_lListenerList) {
    if (pListener->isGameInputListenerEnabled()) {
      pListener->receiveInputCommand(cmd);
    }
  }
}
