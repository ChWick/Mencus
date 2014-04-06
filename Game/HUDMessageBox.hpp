#ifndef _HUD_MESSAGE_BOX_HPP_
#define _HUD_MESSAGE_BOX_HPP_

#include <CEGUI/CEGUI.h>
#include "HUDElement.hpp"

class CHUDMessageBox : public CHUDElement {
private:
  CEGUI::Window *m_pMessageBox;

public:
  CHUDMessageBox(const char *pTitle, const char *pText);
  ~CHUDMessageBox();

  bool onCloseButtonClicked(const CEGUI::EventArgs &args);
};

#endif
