#include "GUIInput.hpp"
#include "GameInputManager.hpp"
#include "GameInputCommand.hpp"
#include <OgreLogManager.h>
#include "Weapon.hpp"

using namespace CEGUI;

CGUIInput::CGUIInput(CEGUI::Window *pGUIRoot)
  : m_uiCurrentWeapon(0) {
  m_pDirectionButtonContainer = pGUIRoot->createChild("DefaultWindow", "ButtonContainer");
  m_pDirectionButtonContainer->setInheritsAlpha(false);

  m_pControlButtonContainer = pGUIRoot->createChild("DefaultWindow", "ControlContainer");
  m_pControlButtonContainer->setInheritsAlpha(false);

  for (int i = 0; i < BT_COUNT; i++) {
    m_pButtons[i] = createButton(i);
  }

  // create drag button
  Window *pDragButton = pGUIRoot->createChild("OgreTray/Button", "DragButton");
  pDragButton->setSize(USize(UDim(1, 0), UDim(0.05, 0)));
  pDragButton->setAlpha(0.5);
  //pDragButton->setProperty("FrameEnabled", "False");
  //pDragButton->setProperty("BackgroundEnabled", "True");
  pDragButton->
    subscribeEvent(
		   CEGUI::Window::EventMouseButtonDown,
		   Event::Subscriber(&CGUIInput::onDragPressed, this));
  pDragButton->
    subscribeEvent(
		   CEGUI::Window::EventMouseButtonUp,
		   Event::Subscriber(&CGUIInput::onDragReleased, this));
  pDragButton->
    subscribeEvent(
		   CEGUI::Window::EventMouseMove,
		   Event::Subscriber(&CGUIInput::onDragMoved, this));
  pDragButton->
    subscribeEvent(
		   Window::EventMouseLeavesArea,
		   Event::Subscriber(&CGUIInput::onDragMoved, this));

  void buttonSizeChanged(float fSize);
  m_pDragButton = pDragButton;

  // create drag frame
  m_pDragWindow = pGUIRoot->createChild("OgreTray/Group", "DragWindow");
  m_pDragWindow->setAlpha(0.8);
  m_pDragWindow->setSize(USize(UDim(1, 0), UDim(0, 200)));

  for (unsigned int i = 0; i < Weapon::W_COUNT; i++) {
    m_pWeapons[i] = createWeaponButton(i);
  }

  setCurrentWeapon(0);
  this->buttonSizeChanged(64);
}
void CGUIInput::buttonSizeChanged(float fSize) {
  m_fButtonSize = fSize;

  // resize direction button container
  m_pDirectionButtonContainer->setPosition(UVector2(UDim(0, 0), UDim(1, -fSize)));
  m_pDirectionButtonContainer->setSize(USize(UDim(0, 2 * fSize), UDim(0, fSize)));

  // resize controll button container
  m_pControlButtonContainer->setPosition(UVector2(UDim(1, -fSize), UDim(1, -4 * fSize)));
  m_pControlButtonContainer->setSize(USize(UDim(0, fSize), UDim(0, 4 * fSize)));

  // resize drag window
  m_pDragWindow->setSize(USize(UDim(1, 0), UDim(0, 2 * fSize)));
  for (unsigned int i = 0; i < Weapon::W_COUNT; i++) {
    m_pWeapons[i]->setSize(USize(UDim(0, fSize), UDim(0, fSize)));
    m_pWeapons[i]->setPosition(UVector2(UDim(0, 10 + i * fSize), UDim(0, 0)));
  }

  // resize push buttons
  for (unsigned int i = 0; i < BT_COUNT; i++) {
    m_pButtons[i]->setSize(USize(UDim(0, fSize), UDim(0, fSize)));
    if (i > BT_RIGHT) {
      m_pButtons[i]->setPosition(UVector2(UDim(0, 0), UDim(0, (i - BT_ENTER_LINK) * fSize)));
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
  case BT_ACTIVATE:
    pButton->
      subscribeEvent(
		     Window::EventMouseEntersArea,
		     Event::Subscriber(&CGUIInput::onActivatePressed, this));
    pButton->
      subscribeEvent(
		     Window::EventMouseLeavesArea,
		     Event::Subscriber(&CGUIInput::onActivateReleased, this));
    break;
  case BT_ATTACK:
    pButton->
      subscribeEvent(
		     Window::EventMouseEntersArea,
		     Event::Subscriber(&CGUIInput::onAttackPressed, this));
    pButton->
      subscribeEvent(
		     Window::EventMouseLeavesArea,
		     Event::Subscriber(&CGUIInput::onAttackReleased, this));
    break;
  }
  
  return pButton;
}
CEGUI::Window *CGUIInput::createWeaponButton(unsigned int uiWeapon) {
  Window *pButton = m_pDragWindow->createChild("OgreTray/StaticImage", "WeaponButton" + PropertyHelper<int>::toString(uiWeapon));
  pButton->setSize(USize(UDim(0, 100), UDim(0, 100)));
  pButton->setAlpha(1);
  
  // this is default for control buttons
  pButton->setPosition(UVector2(UDim(0, 10 + uiWeapon * 100), UDim(0, 0)));
  pButton->setProperty("Image", Weapon::getPicture(uiWeapon));

  pButton->
    subscribeEvent(Window::EventMouseButtonDown,
		   Event::Subscriber(&CGUIInput::onWeaponClick, this));


    pButton->setProperty("BackgroundEnabled", "False");
    pButton->setProperty("FrameEnabled", "False");

  return pButton;
}
void CGUIInput::update(float tpf) {
  if (m_eDragState == DS_CLOSING) {
    m_pDragButton->setPosition(m_pDragButton->getPosition() +
			       UVector2(UDim(0, 0), UDim(0, -tpf * 10 * m_fButtonSize)));
  }
  else if (m_eDragState == DS_OPENING) {
    m_pDragButton->setPosition(m_pDragButton->getPosition() +
			       UVector2(UDim(0, 0), UDim(0, tpf * 10 * m_fButtonSize)));
  }

  if (m_pDragButton->getPosition().d_y.d_offset > 2 * m_fButtonSize) {
    m_pDragButton->setPosition(UVector2(UDim(0, 0), UDim(-0.025, 2 * m_fButtonSize)));
    if (m_eDragState == DS_OPENING) {
      m_eDragState = DS_OPEN;
    }
  }
  else if (m_pDragButton->getPosition().d_y.d_offset < 0) {
    m_pDragButton->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    if (m_eDragState == DS_CLOSING) {
      m_eDragState = DS_SLEEPING;
    }
  }

  m_fDragVelocity = (m_pDragButton->getPosition().d_y.d_offset - m_fLastDragPos) / tpf;
  m_fLastDragPos = m_pDragButton->getPosition().d_y.d_offset;

  m_pDragWindow->setPosition(UVector2(UDim(0, 0), UDim(-0.025, m_fLastDragPos - 2 * m_fButtonSize)));
}
void CGUIInput::setCurrentWeapon(unsigned int uiWeapon) {
  m_pWeapons[m_uiCurrentWeapon]->setProperty("BackgroundEnabled", "False");
  m_pWeapons[m_uiCurrentWeapon]->setProperty("FrameEnabled", "False");
  m_pWeapons[uiWeapon]->setProperty("BackgroundEnabled", "True");
  m_pWeapons[uiWeapon]->setProperty("FrameEnabled", "True");
  m_uiCurrentWeapon = uiWeapon;
}
void CGUIInput::updateDragButtonPosition(const CEGUI::EventArgs& args) {
const CEGUI::MouseEventArgs &mea = dynamic_cast<const CEGUI::MouseEventArgs&>(args);
 updateDragBar(mea.position.d_y);
}
void CGUIInput::updateDragBar(float fPosY) {
  if (m_eDragState == DS_DRAGGING) {
    m_pDragButton->setPosition(UVector2(UDim(0, 0), UDim(-0.025, fPosY)));
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
bool CGUIInput::onAttackPressed(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_ATTACK, 1));
  return true;
}
bool CGUIInput::onAttackReleased(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_ATTACK, 0));
  return true;
}
bool CGUIInput::onActivatePressed(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_ACTIVATE, 1));
  return true;
}
bool CGUIInput::onActivateReleased(const CEGUI::EventArgs&) {
  CGameInputManager::getSingleton().
    sendCommandToListeners(CGameInputCommand(GIC_ACTIVATE, 0));
  return true;
}
bool CGUIInput::onDragPressed(const CEGUI::EventArgs&) {
  pause(PAUSE_MAP_UPDATE);
  m_eDragState = DS_DRAGGING;
  return true;
}
bool CGUIInput::onDragReleased(const CEGUI::EventArgs&) {
  if (m_fDragVelocity < 0) {
    if (m_fDragVelocity < -10 || m_fLastDragPos < 70) {
      m_eDragState = DS_CLOSING;
      unpause(PAUSE_MAP_UPDATE);
    }
    else {
      m_eDragState = DS_OPENING;
    }
  }
  else {
    if (m_fDragVelocity > 10 || m_fLastDragPos > m_fButtonSize * 2 - 70) {
      m_eDragState = DS_OPENING;
    }
    else {
      m_eDragState = DS_CLOSING;
      unpause(PAUSE_MAP_UPDATE);
    }
  }
  return true;
}
bool CGUIInput::onDragMoved(const CEGUI::EventArgs& args) {
  updateDragButtonPosition(args);
  
  return true;
}
bool CGUIInput::onWeaponClick(const CEGUI::EventArgs& args) {
  CEGUI::Window *pBtn = dynamic_cast<const WindowEventArgs*>(&args)->window;
  for (unsigned int i = 0; i < Weapon::W_COUNT; i++) {
    if (m_pWeapons[i] == pBtn) {
      setCurrentWeapon(i);
      break;
    }
  }
  return true;
}
