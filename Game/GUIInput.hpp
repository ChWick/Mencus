#ifndef GUIINPUT_HPP
#define GUIINPUT_HPP

#include <CEGUI/CEGUI.h>

class CGUIInput {
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
  };
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pDirectionButtonContainer;
  CEGUI::Window *m_pControlButtonContainer;
  CEGUI::Window *m_pDragButton;

  EDragState m_eDragState;
public:
  CGUIInput(CEGUI::Window *pGUIRoot);

  void update(float tpf);

private:
  CEGUI::Window *createButton(int bt);

  void updateDragButtonPosition(const CEGUI::EventArgs&);
  void updateDragBar(float fPosY);

  bool onMouseEntersRightButton(const CEGUI::EventArgs&);
  bool onMouseLeavesRightButton(const CEGUI::EventArgs&);
  bool onMouseMoveRightButton(const CEGUI::EventArgs&);

  bool onMouseEntersLeftButton(const CEGUI::EventArgs&);
  bool onMouseLeavesLeftButton(const CEGUI::EventArgs&);
  bool onMouseMoveLeftButton(const CEGUI::EventArgs&);

  bool onJumpPressed(const CEGUI::EventArgs&);
  bool onJumpReleased(const CEGUI::EventArgs&);

  bool onEnterLinkPressed(const CEGUI::EventArgs&);
  bool onEnterLinkReleased(const CEGUI::EventArgs&);

  bool onAttackPressed(const CEGUI::EventArgs&);
  bool onAttackReleased(const CEGUI::EventArgs&);

  bool onActivatePressed(const CEGUI::EventArgs&);
  bool onActivateReleased(const CEGUI::EventArgs&);

  bool onDragPressed(const CEGUI::EventArgs&);
  bool onDragReleased(const CEGUI::EventArgs&);
  bool onDragMoved(const CEGUI::EventArgs&);
};

#endif
