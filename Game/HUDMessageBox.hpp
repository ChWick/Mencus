#ifndef _HUD_MESSAGE_BOX_HPP_
#define _HUD_MESSAGE_BOX_HPP_

#include <CEGUI/CEGUI.h>
#include "HUDElement.hpp"
#include "PauseCaller.hpp"

class CHUDMessageBox
  : public CHUDElement,
    public CPauseCaller {
private:
  CEGUI::Window *m_pMessageBox;
  const std::vector<std::string> m_vPages;
  unsigned int m_uiCurrentPage;
public:
  CHUDMessageBox(const char *pTitle, const std::vector<std::string> &vPages);
  ~CHUDMessageBox();

  bool onCloseButtonClicked(const CEGUI::EventArgs &args);

private:
  void showPage(unsigned int uiPage);
};

#endif
