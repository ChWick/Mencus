#include "GUIInput.hpp"
#include "GameInputManager.hpp"
#include "GameInputCommand.hpp"
#include <OgreLogManager.h>
#include "Weapon.hpp"
#include "Game.hpp"
#include "GUIManager.hpp"

using namespace CEGUI;

CGUIInput::CGUIInput(CEGUI::Window *pGUIRoot)
  : m_uiCurrentWeapon(0) {
  CInputListenerManager::getSingleton().addInputListener(this);

  m_pDirectionButtonContainer = pGUIRoot->createChild("DefaultWindow", "ButtonContainer");
  m_pDirectionButtonContainer->setInheritsAlpha(false);

  m_pControlButtonContainer = pGUIRoot->createChild("DefaultWindow", "ControlContainer");
  m_pControlButtonContainer->setInheritsAlpha(false);

  for (int i = 0; i < BT_COUNT; i++) {
    m_pButtons[i] = createButton(i);
  }

  // create drag button
  Window *pDragButton = pGUIRoot->createChild("OgreTray/StaticImage", "DragButton");
  pDragButton->setSize(USize(UDim(1, 0), UDim(0, 50)));
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

  pDragButton->setProperty("HorzFormatting", "Tiled");
  pDragButton->setProperty("VertFormatting", "CentreAligned");
  //pDragButton->setProperty("Image", "OgreTrayImages/SeparatorM");
  m_pDragButton = pDragButton;

  // create drag frame
  m_pDragWindow = pGUIRoot->createChild("OgreTray/Group", "DragWindow");
  m_pDragWindow->setText("Tools");
  m_pDragWindow->setFont("dejavusans12");
  //m_pDragWindow->setProperty("FrameEnabled", "False");
  m_pDragWindow->setAlpha(0.8);
  m_pDragWindow->setSize(USize(UDim(1, 0), UDim(0, 1.5 * 100)));

  for (unsigned int i = 0; i < Weapon::I_COUNT; i++) {
    m_pWeapons[i] = createWeaponButton(i);
    m_pWeaponLabels[i] = NULL;
  }

  createWeaponButtonLabel(Weapon::I_BOMB);
  createWeaponButtonLabel(Weapon::I_HEALTH_POTION);
  createWeaponButtonLabel(Weapon::I_MANA_POTION);
  createWeaponButtonLabel(Weapon::I_KEY);

  m_pDirectionButtonContainer->setAlwaysOnTop(true);
  m_pControlButtonContainer->setAlwaysOnTop(true);
  m_pDragButton->setAlwaysOnTop(true);
  m_pDragWindow->setAlwaysOnTop(true);
  m_pDragButton->setAlpha(0);

  setCurrentWeapon(0);
  this->buttonSizeChanged(85);
}
CGUIInput::~CGUIInput() {
  CInputListenerManager::getSingleton().removeInputListener(this);
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
  for (unsigned int i = 0; i < Weapon::I_COUNT; i++) {
    m_pWeapons[i]->setSize(USize(UDim(0, fSize), UDim(0, fSize)));
    m_pWeapons[i]->setPosition(UVector2(UDim(0, i * fSize), UDim(0, 0)));
  }

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

  // resize pull menu
  m_pDragWindow->setSize(USize(UDim(1, 0), UDim(0, 1.5 * fSize)));
  m_pDragButton->setSize(USize(UDim(1, 0), UDim(0, 0.5 * fSize)));
  
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
CEGUI::Window *CGUIInput::createWeaponButton(unsigned int uiWeapon) {
  Window *pButton = m_pDragWindow->createChild("OgreTray/StaticImage", "WeaponButton" + PropertyHelper<int>::toString(uiWeapon));
  pButton->setSize(USize(UDim(0, 100), UDim(0, 100)));
  pButton->setAlpha(1);
  
  // this is default for control buttons
  pButton->setPosition(UVector2(UDim(0, uiWeapon * 100), UDim(0, 0)));
  pButton->setProperty("Image", Weapon::getPicture(uiWeapon));

  pButton->
    subscribeEvent(Window::EventMouseButtonDown,
		   Event::Subscriber(&CGUIInput::onWeaponClick, this));


    pButton->setProperty("BackgroundEnabled", "False");
    pButton->setProperty("FrameEnabled", "False");

  return pButton;
}
CEGUI::Window *CGUIInput::createWeaponButtonLabel(unsigned int uiWeapon) {
  Window *pButton = m_pWeapons[uiWeapon]->createChild("OgreTray/Label", "Label" + PropertyHelper<int>::toString(uiWeapon));
  pButton->setSize(USize(UDim(1,0),UDim(0.8,0)));
  pButton->setText("0");
  pButton->setProperty("NormalTextColour", "FFFFFFFF");
  pButton->setProperty("VertFormatting", "BottomAligned");
  pButton->setFont("dejavusans12");
  pButton->
    subscribeEvent(Window::EventMouseButtonDown,
		   Event::Subscriber(&CGUIInput::onWeaponClick, this));
  
  m_pWeaponLabels[uiWeapon] = pButton;

  return pButton;
}
void CGUIInput::update(float tpf) {
  if (m_eDragState == DS_DRAGGING || m_eDragState == DS_OPEN || m_eDragState == DS_OPENING) {
    m_pDragButton->setAlpha(0.5);
  }
  else {
    m_pDragButton->setAlpha(0);
  }

  if (m_eDragState == DS_CLOSING) {
    m_pDragButton->setPosition(m_pDragButton->getPosition() +
			       UVector2(UDim(0, 0), UDim(0, -tpf * 10 * m_fButtonSize)));
  }
  else if (m_eDragState == DS_OPENING) {
    m_pDragButton->setPosition(m_pDragButton->getPosition() +
			       UVector2(UDim(0, 0), UDim(0, tpf * 10 * m_fButtonSize)));
  }

  if (m_pDragButton->getPosition().d_y.d_offset > m_pDragWindow->getSize().d_height.d_offset) {
    m_pDragButton->setPosition(UVector2(UDim(0, 0), UDim(0, m_pDragWindow->getSize().d_height.d_offset)));
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

  m_pDragWindow->setPosition(UVector2(UDim(0, 0), UDim(0, m_fLastDragPos - m_pDragWindow->getSize().d_height.d_offset)));

  updateInput();
}
void CGUIInput::setCurrentWeapon(unsigned int uiWeapon) {
  m_pWeapons[m_uiCurrentWeapon]->setProperty("BackgroundEnabled", "False");
  m_pWeapons[m_uiCurrentWeapon]->setProperty("FrameEnabled", "False");
  m_pWeapons[uiWeapon]->setProperty("BackgroundEnabled", "True");
  m_pWeapons[uiWeapon]->setProperty("FrameEnabled", "True");
  m_uiCurrentWeapon = uiWeapon;
  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_CHANGE_WEAPON, uiWeapon));

  // set the image of the touch button
  m_pButtons[BT_ATTACK]->setProperty("Image", Weapon::getPicture(uiWeapon));
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
    std::vector<OIS::MultiTouchState> mts = pMultiTouch->getMultiTouchStates();
    for (auto &state : mts) {
      if (state.touchType == OIS::MT_Moved) {
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
void CGUIInput::updateDragButtonPosition(const CEGUI::EventArgs& args) {
const CEGUI::MouseEventArgs &mea = dynamic_cast<const CEGUI::MouseEventArgs&>(args);
 updateDragBar(mea.position.d_y);
}
void CGUIInput::updateDragBar(float fPosY) {
  if (m_eDragState == DS_DRAGGING) {
    m_pDragButton->setPosition(UVector2(UDim(0, 0), UDim(0, fPosY - 0.25 * m_fButtonSize)));
  }
}
bool CGUIInput::onDragPressed(const CEGUI::EventArgs&) {
  pause(PAUSE_MAP_UPDATE);
  m_eDragState = DS_DRAGGING;
  return true;
}
bool CGUIInput::onDragReleased(const CEGUI::EventArgs&) {
  pressReleased();
  return true;
}
bool CGUIInput::onDragMoved(const CEGUI::EventArgs& args) {
  updateDragButtonPosition(args);
  
  return true;
}
bool CGUIInput::onWeaponClick(const CEGUI::EventArgs& args) {
  CEGUI::Window *pBtn = dynamic_cast<const WindowEventArgs*>(&args)->window;

  if (m_pWeapons[Weapon::I_HEALTH_POTION] == pBtn ||
      m_pWeaponLabels[Weapon::I_HEALTH_POTION] == pBtn) {
    CGameInputManager::getSingleton().
      injectCommand(CGameInputCommand(GIC_USE_HEALTH_POTION));
    return true;
  }
  else if (m_pWeapons[Weapon::I_MANA_POTION] == pBtn ||
	   m_pWeaponLabels[Weapon::I_MANA_POTION] == pBtn) {
    CGameInputManager::getSingleton().
      injectCommand(CGameInputCommand(GIC_USE_MANA_POTION));
    return true;
  }

  for (unsigned int i = 0; i < Weapon::W_COUNT; i++) {
    if (m_pWeapons[i] == pBtn || m_pWeaponLabels[i] == pBtn) {
      setCurrentWeapon(i);
      return true;
    }
  }
  
  return true;
}
void CGUIInput::pressReleased() {
  if (m_eDragState != DS_DRAGGING) {
    return;
  }
  if (m_fDragVelocity < 0) {
    if (m_fDragVelocity < -2 || m_fLastDragPos < 70) {
      m_eDragState = DS_CLOSING;
      unpause(PAUSE_MAP_UPDATE);
    }
    else {
      m_eDragState = DS_OPENING;
    }
  }
  else {
    if (m_fDragVelocity > 2 || m_fLastDragPos > m_fButtonSize * 2 - 70) {
      m_eDragState = DS_OPENING;
    }
    else {
      m_eDragState = DS_CLOSING;
      unpause(PAUSE_MAP_UPDATE);
    }
  }
}
void CGUIInput::show() {
  m_pDirectionButtonContainer->setVisible(true);
  m_pControlButtonContainer->setVisible(true);
  m_pDragButton->setVisible(true);
  m_pDragWindow->setVisible(true);
}
void CGUIInput::hide() {
  m_pDirectionButtonContainer->setVisible(false);
  m_pControlButtonContainer->setVisible(false);
  m_pDragButton->setVisible(false);
  m_pDragWindow->setVisible(false);
}
