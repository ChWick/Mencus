#include "GameInputListener.hpp"
#include "GameInputManager.hpp"
CGameInputListener::CGameInputListener(bool bDefaultEnabled)
  : m_bGameInputListenerEnabled(bDefaultEnabled) {
  CGameInputManager::getSingleton().addInputListener(this);
}
CGameInputListener::~CGameInputListener() {
  CGameInputManager::getSingleton().removeInputListener(this);
}
