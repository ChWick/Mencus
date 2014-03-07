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

  CEGUI::Window *pLeftButton = createDirectionButton(-1);
  Window *pRightButton = createDirectionButton(1);

  
}
CEGUI::Window *CGUIInput::createDirectionButton(int iDir) {
  Window *pButton = m_pDirectionButtonContainer->createChild("OgreTray/Button", (iDir > 0) ? "right" : "left");
  pButton->setSize(USize(UDim(0.5, 0), UDim(1, 0)));
  pButton->setPosition(UVector2(UDim((iDir > 0) ? 0.5 : 0, 0), UDim(0, 0)));

  pButton->
    subscribeEvent(
		   Window::EventMouseEntersArea,
		   Event::Subscriber(
				     (iDir > 0) ?
				     &CGUIInput::onMouseEntersRightButton :
				     &CGUIInput::onMouseEntersLeftButton,
				     this));
  pButton->
    subscribeEvent(
		   Window::EventMouseLeavesArea,
		   Event::Subscriber(
				     (iDir > 0) ?
				     &CGUIInput::onMouseLeavesRightButton :
				     &CGUIInput::onMouseLeavesLeftButton,
				     this));
  pButton->
    subscribeEvent(
		   Window::EventMouseMove,
		   Event::Subscriber(
				     (iDir > 0) ?
				     &CGUIInput::onMouseMoveRightButton :
				     &CGUIInput::onMouseMoveLeftButton,
				     this));
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
