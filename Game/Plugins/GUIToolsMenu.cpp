#include "GUIToolsMenu.hpp"
#include "MessageHandler.hpp"
#include "GameInputCommand.hpp"
#include "GameInputManager.hpp"

using namespace CEGUI;

CGUIToolsMenu::CGUIToolsMenu(CEGUI::Window *pParent)
  : m_pRoot(nullptr),
    m_eDragState(DS_SLEEPING),
    m_bPressed(false),
    m_fTimeSinceLastTouchMoveEvent(0),
    m_uiCurrentWeapon(1),
    m_fTimer(0),
    m_pDragArrow(nullptr) {
  CInputListenerManager::getSingleton().addInputListener(this);
  CMessageHandler::getSingleton().addInjector(this);


  m_pRoot = pParent;
  // create drag button
  Window *pDragButton = m_pRoot->createChild("OgreTray/StaticImage", "DragButton");
  pDragButton->setSize(USize(UDim(1, 0), UDim(0, 50)));
  pDragButton->setAlpha(0.5);
#if INPUT_TOUCH
  //pDragButton->setProperty("FrameEnabled", "False");
  //pDragButton->setProperty("BackgroundEnabled", "True");
  pDragButton->
    subscribeEvent(
		   CEGUI::Window::EventMouseButtonDown,
		   Event::Subscriber(&CGUIToolsMenu::onDragPressed, this));
  pDragButton->
    subscribeEvent(
		   CEGUI::Window::EventMouseEntersArea,
		   Event::Subscriber(&CGUIToolsMenu::onDragEnter, this));
  pDragButton->
    subscribeEvent(
		   CEGUI::Window::EventMouseButtonUp,
		   Event::Subscriber(&CGUIToolsMenu::onDragReleased, this));
  pDragButton->
    subscribeEvent(
		   CEGUI::Window::EventMouseMove,
		   Event::Subscriber(&CGUIToolsMenu::onDragMoved, this));
  pDragButton->
    subscribeEvent(
		   Window::EventMouseLeavesArea,
		   Event::Subscriber(&CGUIToolsMenu::onDragMoved, this));

  pDragButton->setProperty("HorzFormatting", "Tiled");
  pDragButton->setProperty("VertFormatting", "CentreAligned");
#endif
  //pDragButton->setProperty("Image", "OgreTrayImages/SeparatorM");
  m_pDragButton = pDragButton;


  // create drag frame
  m_pDragWindow = m_pRoot->createChild("OgreTray/Group", "DragWindow");
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

  m_pDragButton->setAlwaysOnTop(true);
  m_pDragWindow->setAlwaysOnTop(true);
  m_pDragWindow->moveToFront();
  m_pDragButton->moveToFront();
  m_pDragButton->setAlpha(0);
  //m_pDragButton->setRiseOnClickEnabled(false);
  m_pDragWindow->setRiseOnClickEnabled(false);
  //m_pDragWindow->setZOrderingEnabled(false);

  setCurrentWeapon(0);

  //hide();
}

CGUIToolsMenu::~CGUIToolsMenu() {
  CInputListenerManager::getSingleton().removeInputListener(this);
  CMessageHandler::getSingleton().removeInjector(this);
}


void CGUIToolsMenu::changeTouchButtonSize(float fSize) {
  m_fButtonSize = fSize;

  // resize drag arrow
  if (m_pDragArrow) {
    m_pDragArrow->setPosition(UVector2(UDim(0.5, -m_fButtonSize * 0.5), UDim(0, 0)));
    m_pDragArrow->setSize(USize(UDim(0, m_fButtonSize), UDim(0, m_fButtonSize)));
  }

  // resize drag window
  m_pDragWindow->setSize(USize(UDim(1, 0), UDim(0, 2 * fSize)));
  for (unsigned int i = 0; i < Weapon::I_COUNT; i++) {
    m_pWeapons[i]->setSize(USize(UDim(0, fSize), UDim(0, fSize)));
    m_pWeapons[i]->setPosition(UVector2(UDim(0, i * fSize), UDim(0, 0)));
  }

  // resize pull menu
  m_pDragWindow->setSize(USize(UDim(1, 0), UDim(0, 1.5 * fSize)));
  m_pDragButton->setSize(USize(UDim(1, 0), UDim(0, 0.5 * fSize)));
}

void CGUIToolsMenu::onGUIScalingChanged(float fScaling) {
}

CEGUI::Window *CGUIToolsMenu::createWeaponButton(unsigned int uiWeapon) {
  Window *pButton = m_pDragWindow->createChild("OgreTray/StaticImage", "WeaponButton" + PropertyHelper<int>::toString(uiWeapon));
  pButton->setSize(USize(UDim(0, 100), UDim(0, 100)));
  pButton->setAlpha(1);

  // this is default for control buttons
  pButton->setPosition(UVector2(UDim(0, uiWeapon * 100), UDim(0, 0)));
  pButton->setProperty("Image", Weapon::getPicture(uiWeapon));

#if INPUT_TOUCH
  pButton->
    subscribeEvent(Window::EventMouseButtonDown,
		   Event::Subscriber(&CGUIToolsMenu::onWeaponClick, this));
#endif


    pButton->setProperty("BackgroundEnabled", "False");
    pButton->setProperty("FrameEnabled", "False");

  return pButton;
}

CEGUI::Window *CGUIToolsMenu::createWeaponButtonLabel(unsigned int uiWeapon) {
  Window *pButton = m_pWeapons[uiWeapon]->createChild("OgreTray/Label", "Label" + PropertyHelper<int>::toString(uiWeapon));
  pButton->setSize(USize(UDim(1,0),UDim(0.8,0)));
  pButton->setText("0");
  pButton->setProperty("NormalTextColour", "FFFFFFFF");
  pButton->setProperty("VertFormatting", "BottomAligned");
  pButton->setFont("dejavusans12");
#if INPUT_TOUCH
  pButton->
    subscribeEvent(Window::EventMouseButtonDown,
		   Event::Subscriber(&CGUIToolsMenu::onWeaponClick, this));
#endif

  m_pWeaponLabels[uiWeapon] = pButton;

  return pButton;
}

void CGUIToolsMenu::update(float tpf) {
  if (m_lBlinkingButtons.size() > 0 || m_pDragArrow) {
    m_fTimer += tpf;
  }
  if (m_lBlinkingButtons.size() > 0) {
    float fColValue = (cos(m_fTimer * 6) + 1) * 0.5;
    for (auto pButton : m_lBlinkingButtons) {
      pButton->setProperty("ImageColours", PropertyHelper<Colour>::toString(Colour(1, fColValue, fColValue)));
    }
  }

  if (m_pDragArrow) {
    float fScaledTime = m_fTimer * 0.75f;
    m_pDragArrow->setPosition(UVector2(UDim(0.5, -m_fButtonSize * 0.5), UDim(0, m_fButtonSize * (fScaledTime - floor(fScaledTime)) * 2)));
  }


  if (m_eDragState == DS_DRAGGING || m_eDragState == DS_OPEN || m_eDragState == DS_OPENING) {
    m_pDragButton->setAlpha(0.5);
    if (m_eDragState == DS_DRAGGING) {
      m_fTimeSinceLastTouchMoveEvent += tpf;
    }
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
      CMessageHandler::getSingleton().addMessage(CMessage(CMessage::MT_DROP_DOWN_MENU).setBool(true));
    }
  }
  else if (m_pDragButton->getPosition().d_y.d_offset < 0) {
    m_pDragButton->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    if (m_eDragState == DS_CLOSING) {
      m_eDragState = DS_SLEEPING;
      CMessageHandler::getSingleton().addMessage(CMessage(CMessage::MT_DROP_DOWN_MENU).setBool(false));
    }
  }

  m_pDragWindow->setPosition(UVector2(UDim(0, 0), UDim(0, m_pDragButton->getPosition().d_y.d_offset - m_pDragWindow->getSize().d_height.d_offset)));
}

void CGUIToolsMenu::setCurrentWeapon(unsigned int uiWeapon) {
  if (m_uiCurrentWeapon == uiWeapon) {return;}

  m_pWeapons[m_uiCurrentWeapon]->setProperty("BackgroundEnabled", "False");
  m_pWeapons[m_uiCurrentWeapon]->setProperty("FrameEnabled", "False");
  m_pWeapons[uiWeapon]->setProperty("BackgroundEnabled", "True");
  m_pWeapons[uiWeapon]->setProperty("FrameEnabled", "True");
  m_uiCurrentWeapon = uiWeapon;
  CGameInputManager::getSingleton().
    injectCommand(CGameInputCommand(GIC_CHANGE_WEAPON, uiWeapon));
}


void CGUIToolsMenu::updateDragButtonPosition(const CEGUI::EventArgs& args) {
const CEGUI::MouseEventArgs &mea = dynamic_cast<const CEGUI::MouseEventArgs&>(args);
 updateDragBar(mea.position.d_y);
}

void CGUIToolsMenu::updateDragBar(float fPosY) {
  if (m_eDragState == DS_DRAGGING) {
    m_pDragButton->setPosition(UVector2(UDim(0, 0), UDim(0, fPosY - 0.25 * m_fButtonSize)));
  }
}

bool CGUIToolsMenu::onDragPressed(const CEGUI::EventArgs&) {
  m_bPressed = true;
  pause(PAUSE_MAP_UPDATE);
  m_eDragState = DS_DRAGGING;
  m_fLastDragPos = m_pDragButton->getPosition().d_y.d_offset;
  return true;
}

bool CGUIToolsMenu::onDragReleased(const CEGUI::EventArgs&) {
  m_bPressed = false;
  pressReleased();
  return true;
}

bool CGUIToolsMenu::onDragMoved(const CEGUI::EventArgs& args) {
  updateDragButtonPosition(args);
  m_fDragVelocity = (m_pDragButton->getPosition().d_y.d_offset - m_fLastDragPos) / m_fTimeSinceLastTouchMoveEvent;
  m_fLastDragPos = m_pDragButton->getPosition().d_y.d_offset;
  m_fTimeSinceLastTouchMoveEvent = 0;
  return true;
}

bool CGUIToolsMenu::onDragEnter(const CEGUI::EventArgs& args) {
  if (m_bPressed) {
    onDragPressed(args);
  }
  return true;
}

bool CGUIToolsMenu::onWeaponClick(const CEGUI::EventArgs& args) {
  CEGUI::Window *pBtn = dynamic_cast<const WindowEventArgs*>(&args)->window;

  if (m_pWeapons[Weapon::I_HEALTH_POTION] == pBtn ||
      m_pWeaponLabels[Weapon::I_HEALTH_POTION] == pBtn) {
    CGameInputManager::getSingleton().
      injectCommand(CGameInputCommand(GIC_USE_HEALTH_POTION, 1, GIS_CLICKED));
    return true;
  }
  else if (m_pWeapons[Weapon::I_MANA_POTION] == pBtn ||
	   m_pWeaponLabels[Weapon::I_MANA_POTION] == pBtn) {
    CGameInputManager::getSingleton().
      injectCommand(CGameInputCommand(GIC_USE_MANA_POTION, 1, GIS_CLICKED));
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

void CGUIToolsMenu::pressReleased() {
  if (m_eDragState != DS_DRAGGING) {
    return;
  }
  //Ogre::LogManager::getSingleton().logMessage(Ogre::StringConverter::toString(m_fDragVelocity));
  if (m_fDragVelocity < 0) {
    if (m_fDragVelocity < -m_pDragWindow->getSize().d_height.d_offset * 0.05 || m_fLastDragPos < m_pDragWindow->getSize().d_height.d_offset * 0.2) {
      m_eDragState = DS_CLOSING;
      unpause(PAUSE_MAP_UPDATE);
    }
    else {
      m_eDragState = DS_OPENING;
    }
  }
  else {
    if (m_fDragVelocity > m_pDragWindow->getSize().d_height.d_offset * 0.05 || m_fLastDragPos > m_pDragWindow->getSize().d_height.d_offset * 0.8) {
      m_eDragState = DS_OPENING;
    }
    else {
      m_eDragState = DS_CLOSING;
      unpause(PAUSE_MAP_UPDATE);
    }
  }
}

void CGUIToolsMenu::sendMessageToAll(const CMessage &message) {
  switch (message.getType()) {
  case CMessage::MT_TOGGLE_TOUCH_INPUT_BLINK:
    {
      if (message.getID() == "drag") {
	// create drag arrow
	if (message.getBool()) {
	  if (!m_pDragArrow) {
	    m_pDragArrow = m_pRoot->createChild("OgreTray/StaticImage", "DragArrow");
	    m_pDragArrow->setProperty("Image", "hud_weapons/down_arrow");
	    m_pDragArrow->setProperty("BackgroundEnabled", "False");
	    m_pDragArrow->setProperty("FrameEnabled", "False");
	    m_pDragArrow->setSize(USize(UDim(0, m_fButtonSize), UDim(0, m_fButtonSize)));
	    m_pDragArrow->setPosition(UVector2(UDim(0.5, -m_fButtonSize * 0.5), UDim(0, 0)));
	    m_pDragArrow->setAlwaysOnTop(true);
	    m_pDragArrow->setInheritsAlpha(false);
	  }
	}
	else {
	  if (m_pDragArrow) {
	    m_pDragArrow->destroy();
	    m_pDragArrow = nullptr;
	  }
	}

	m_fTimer = 0;
      }
      else {
	CEGUI::Window *pButton = nullptr;
	Weapon::EItems item = Weapon::parseItem(message.getID());
	if (item != Weapon::I_COUNT) {
	  pButton = m_pWeapons[item];
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
    }
    break;
  case CMessage::MT_MAP_DESTROYED:
    for (int i = 0; i < Weapon::I_COUNT; i++) {
      m_pWeapons[i]->setProperty("ImageColours", "FFFFFFFF");
    }
    m_lBlinkingButtons.clear();
    m_fTimer = 0;
    break;
  default:
    break;
  }
}


void CGUIToolsMenu::receiveInputCommand(const CGameInputCommand &cmd) {
  switch (cmd.getType()) {
  case GIC_SHOW_MENU:
    if (cmd.getState() == GIS_PRESSED) {
      if (m_eDragState == DS_OPEN || m_eDragState == DS_OPENING) {
        m_eDragState = DS_CLOSING;
	unpause(PAUSE_MAP_UPDATE);
      }
      else {
        m_eDragState = DS_OPENING;
	pause(PAUSE_MAP_UPDATE);
      }
    }
    break;
  case GIC_RIGHT:
    if (m_eDragState == DS_OPEN) {
      if (cmd.getState() == GIS_PRESSED) {
	setCurrentWeapon(m_uiCurrentWeapon + 1);
      }
    }
    break;
  case GIC_LEFT:
    if (m_eDragState == DS_OPEN) {
      if (cmd.getState() == GIS_PRESSED) {
	setCurrentWeapon(m_uiCurrentWeapon - 1);
      }
    }
    break;
  default:
    break;
  }
}

void CGUIToolsMenu::setItemCount(Weapon::EItems eItem, unsigned int uiCount) {
  if (m_pWeaponLabels[eItem]) {
    m_pWeaponLabels[eItem]->setText(PropertyHelper<int>::toString(uiCount));
  }
}

void CGUIToolsMenu::show() {
  m_pDragButton->setVisible(true);
  m_pDragWindow->setVisible(true);
  if (m_pDragArrow) {m_pDragArrow->setVisible(true);}
}
void CGUIToolsMenu::hide() {
  m_pDragButton->setVisible(false);
  m_pDragWindow->setVisible(false);
  if (m_pDragArrow) {m_pDragArrow->setVisible(false);}
}
