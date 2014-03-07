#include "GUIInput.hpp"
#include "GameInputManager.hpp"
#include "GameInputCommand.hpp"
#include <OgreLogManager.h>

using namespace CEGUI;


CGUIInput::CGUIInput(CEGUI::Window *pGUIRoot) {
  m_pDirectionButtonContainer = pGUIRoot->createChild("DefaultWindow", "ButtonContainer");
  m_pDirectionButtonContainer->setInheritsAlpha(false);
  m_pDirectionButtonContainer->setPosition(UVector2(UDim(0, 10), UDim(1, -110)));
  m_pDirectionButtonContainer->setSize(USize(UDim(0, 200), UDim(0, 100)));

  m_pControlButtonContainer = pGUIRoot->createChild("DefaultWindow", "ControlContainer");
  m_pControlButtonContainer->setInheritsAlpha(false);
  m_pControlButtonContainer->setPosition(UVector2(UDim(1, -110), UDim(1, -310)));
  m_pControlButtonContainer->setSize(USize(UDim(0, 100), UDim(0, 300)));

  for (int i = 0; i < BT_COUNT; i++) {
    createButton(i);
  }
}
CEGUI::Window *CGUIInput::createButton(int bt) {
  Window *pParent = m_pControlButtonContainer;
  if (bt <= BT_RIGHT) {pParent = m_pDirectionButtonContainer;}
  Window *pButton = pParent->createChild("OgreTray/Button", "Button" + PropertyHelper<int>::toString(bt));
  pButton->setSize(USize(UDim(0, 100), UDim(0, 100)));
  pButton->setAlpha(0.7);
  
  // this is default for control buttons
  pButton->setPosition(UVector2(UDim(0, 0), UDim(0, (bt - BT_JUMP) * 100)));
  
  switch (bt) {
  case BT_LEFT:
  case BT_RIGHT:
    pButton->setPosition(UVector2(UDim((bt == BT_RIGHT) ? 0.5 : 0, 0), UDim(0, 0)));

    pButton->
      subscribeEvent(
		     Window::EventMouseEntersArea,
		     Event::Subscriber(
				       (bt == BT_RIGHT) ?
				       &CGUIInput::onMouseEntersRightButton :
				       &CGUIInput::onMouseEntersLeftButton,
				       this));
    pButton->
      subscribeEvent(
		     Window::EventMouseLeavesArea,
		     Event::Subscriber(
				       (bt == BT_RIGHT) ?
				       &CGUIInput::onMouseLeavesRightButton :
				       &CGUIInput::onMouseLeavesLeftButton,
				       this));
    pButton->
      subscribeEvent(
		     Window::EventMouseMove,
		     Event::Subscriber(
				       (bt == BT_RIGHT) ?
				       &CGUIInput::onMouseMoveRightButton :
				       &CGUIInput::onMouseMoveLeftButton,
				       this));
    break;
  case BT_JUMP:
    pButton->
      subscribeEvent(
		     Window::EventMouseEntersArea,
		     Event::Subscriber(&CGUIInput::onJumpPressed, this));
    pButton->
      subscribeEvent(
		     Window::EventMouseLeavesArea,
		     Event::Subscriber(&CGUIInput::onJumpReleased, this));
    break;
  case BT_ENTER_LINK:
    pButton->
      subscribeEvent(
		     Window::EventMouseEntersArea,
		     Event::Subscriber(&CGUIInput::onEnterLinkPressed, this));
    pButton->
      subscribeEvent(
		     Window::EventMouseLeavesArea,
		     Event::Subscriber(&CGUIInput::onEnterLinkReleased, this));
    break;
  }
}
bool CGUIInput::onMouseEntersRightButton(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_RIGHT, 1));

  return true;
}
bool CGUIInput::onMouseLeavesRightButton(const CEGUI::EventArgs& args) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_RIGHT, 0));

  return true;
}
bool CGUIInput::onMouseMoveRightButton(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_RIGHT, 1));
  return true;
}
bool CGUIInput::onMouseEntersLeftButton(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_LEFT, 1));
  return true;
}
bool CGUIInput::onMouseLeavesLeftButton(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_LEFT, 0));
  return true;
}
bool CGUIInput::onMouseMoveLeftButton(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_LEFT, 1));
  return true;
}
bool CGUIInput::onJumpPressed(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_JUMP, 1));
  return true;
}
bool CGUIInput::onJumpReleased(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_JUMP, 0));
  return true;
}
bool CGUIInput::onEnterLinkPressed(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_ENTER_LINK, 1));
  return true;
}
bool CGUIInput::onEnterLinkReleased(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_ENTER_LINK, 0));
  return true;
}
