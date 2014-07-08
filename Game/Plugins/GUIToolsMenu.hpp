#ifndef _GUI_TOOLS_MENU_HPP_
#define _GUI_TOOLS_MENU_HPP_

#include "Plugins/GUIOverlay.hpp"
#include "GameInputListener.hpp"
#include "PauseCaller.hpp"
#include "MessageInjector.hpp"
#include "Weapon.hpp"
#include "InputListener.hpp"
#include <CEGUI/CEGUI.h>

class CGUIToolsMenu

  : public CGUIOverlay,
    public CPauseCaller,
    public CMessageInjector,
    public CGameInputListener,
    public CInputListener {

private:

  enum EDragState {
    DS_SLEEPING,
    DS_DRAGGING,
    DS_CLOSING,
    DS_OPENING,
    DS_OPEN,
  };

private:

  CEGUI::Window *m_pRoot;
  
  CEGUI::Window *m_pDragButton;
  CEGUI::Window *m_pDragWindow;
  CEGUI::Window *m_pWeapons[Weapon::I_COUNT];
  CEGUI::Window *m_pWeaponLabels[Weapon::I_COUNT];

  EDragState m_eDragState;
  float m_fDragVelocity;
  float m_fLastDragPos;
  bool m_bPressed;		// !< is the button pressed (mouse or touch)

  float m_fTimeSinceLastTouchMoveEvent;
  float m_fButtonSize;

  unsigned int m_uiCurrentWeapon;

  // for making buttons blink
  float m_fTimer;
  std::list<CEGUI::Window *> m_lBlinkingButtons;

  // arrow to indicate dragging of the pull down menu
  CEGUI::Window *m_pDragArrow;

public:
  CGUIToolsMenu(CEGUI::Window *pParent);
  virtual ~CGUIToolsMenu();

  void update(float tpf);


  void receiveInputCommand(const CGameInputCommand &cmd);

  void setItemCount(Weapon::EItems eItem, unsigned int uiCount);
  void setCurrentWeapon(unsigned int uiCurrentWeapon);

  bool touchReleased(const OIS::MultiTouchEvent& evt) {pressReleased(); return true;}
  bool touchCancelled(const OIS::MultiTouchEvent& evt) {pressReleased();return true;}
  bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {pressReleased(); return true;}

  void show();
  void hide();


  void sendMessageToAll(const CMessage &message);


  void changeTouchButtonSize(float fSize);
  void onGUIScalingChanged(float fScaling);
private:

  CEGUI::Window *createWeaponButton(unsigned int uiWeapon);
  CEGUI::Window *createWeaponButtonLabel(unsigned int uiWeapon);


  void updateDragButtonPosition(const CEGUI::EventArgs&);
  void updateDragBar(float fPosY);

  bool onDragPressed(const CEGUI::EventArgs&);
  bool onDragReleased(const CEGUI::EventArgs&);
  bool onDragMoved(const CEGUI::EventArgs&);
  bool onDragEnter(const CEGUI::EventArgs&);

  bool onWeaponClick(const CEGUI::EventArgs&);


  void pressReleased();
};

#endif
