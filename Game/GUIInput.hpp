#ifndef GUIINPUT_HPP
#define GUIINPUT_HPP

#include <CEGUI/CEGUI.h>
#include <OgreVector2.h>
#include "Weapon.hpp"
#include "PauseCaller.hpp"
#include "InputListener.hpp"

class CGUIInput : public CPauseCaller, public CInputListener {
private:
  enum EButtonTypes {
    BT_LEFT,
    BT_RIGHT,
    BT_ENTER_LINK,
    BT_ACTIVATE,
    BT_JUMP,
    BT_ATTACK,

    BT_COUNT
  };
  enum EDragState {
    DS_SLEEPING,
    DS_DRAGGING,
    DS_CLOSING,
    DS_OPENING,
    DS_OPEN,
  };
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pDirectionButtonContainer;
  CEGUI::Window *m_pControlButtonContainer;
  CEGUI::Window *m_pDragButton;
  CEGUI::Window *m_pDragWindow;
  CEGUI::Window *m_pWeapons[Weapon::I_COUNT];
  CEGUI::Window *m_pWeaponLabels[Weapon::I_COUNT];
  CEGUI::Window *m_pButtons[BT_COUNT];
  Ogre::Vector2 m_vButtonOrigins[BT_COUNT];
  bool m_bButtonPressed[BT_COUNT];

  unsigned int m_uiCurrentWeapon;

  EDragState m_eDragState;
  float m_fDragVelocity;
  float m_fLastDragPos;

  float m_fButtonSize;
public:
  CGUIInput(CEGUI::Window *pGUIRoot);
  ~CGUIInput();

  void update(float tpf);

  bool touchReleased(const OIS::MultiTouchEvent& evt) {pressReleased(); return true;}
  bool touchCancelled(const OIS::MultiTouchEvent& evt) {pressReleased();return true;}
  bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {pressReleased(); return true;}

  void setItemCount(Weapon::EItems eItem, unsigned int uiCount);
  void setCurrentWeapon(unsigned int uiCurrentWeapon);

  void show();
  void hide();

  void windowResized();

  void buttonSizeChanged(float fSize);
private:
  
  void updateInput();
  void checkForButtonPress(const Ogre::Vector2 &vPos);
  int getButtonAtPos(const Ogre::Vector2 &vPos);

  CEGUI::Window *createButton(int bt);
  CEGUI::Window *createWeaponButton(unsigned int uiWeapon);
  CEGUI::Window *createWeaponButtonLabel(unsigned int uiWeapon);


  void updateDragButtonPosition(const CEGUI::EventArgs&);
  void updateDragBar(float fPosY);

  bool onDragPressed(const CEGUI::EventArgs&);
  bool onDragReleased(const CEGUI::EventArgs&);
  bool onDragMoved(const CEGUI::EventArgs&);

  bool onWeaponClick(const CEGUI::EventArgs&);


  void pressReleased();
};

#endif
