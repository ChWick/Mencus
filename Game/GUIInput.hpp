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
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pDirectionButtonContainer;
  CEGUI::Window *m_pControlButtonContainer;
public:
  CGUIInput(CEGUI::Window *pGUIRoot);

private:
  CEGUI::Window *createButton(int bt);

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
};

#endif
