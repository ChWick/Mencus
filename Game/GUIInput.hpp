#ifndef GUIINPUT_HPP
#define GUIINPUT_HPP

#include <CEGUI/CEGUI.h>

class CGUIInput {
private:
  enum EButtonTypes {
    BT_LEFT,
    BT_RIGHT,
    BT_JUMP,
    BT_ENTER_LINK,

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
};

#endif
