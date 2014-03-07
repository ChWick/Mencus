#ifndef GUIINPUT_HPP
#define GUIINPUT_HPP

#include <CEGUI/CEGUI.h>

class CGUIInput {
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pDirectionButtonContainer;
public:
  CGUIInput(CEGUI::Window *pGUIRoot);

private:
  CEGUI::Window *createDirectionButton(int iDir);

  bool onMouseEntersRightButton(const CEGUI::EventArgs&);
  bool onMouseLeavesRightButton(const CEGUI::EventArgs&);
  bool onMouseMoveRightButton(const CEGUI::EventArgs&);

  bool onMouseEntersLeftButton(const CEGUI::EventArgs&);
  bool onMouseLeavesLeftButton(const CEGUI::EventArgs&);
  bool onMouseMoveLeftButton(const CEGUI::EventArgs&);
};

#endif
