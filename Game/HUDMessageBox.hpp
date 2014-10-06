/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef _HUD_MESSAGE_BOX_HPP_
#define _HUD_MESSAGE_BOX_HPP_

#include <CEGUI/CEGUI.h>
#include "HUDElement.hpp"
#include "PauseCaller.hpp"

class CHUDMessageBox
  : public CHUDElement,
    public CPauseCaller {
private:
  const std::string m_sID;
  CEGUI::Window *m_pRootWindow;
  CEGUI::Window *m_pMessageBox;
  const std::vector<std::string> m_vPages;
  unsigned int m_uiCurrentPage;
public:
  CHUDMessageBox(const std::string &sID, const CEGUI::String &sTitle, const std::vector<std::string> &vPages);
  ~CHUDMessageBox();

  bool onCloseButtonClicked(const CEGUI::EventArgs &args);

private:
  void showPage(unsigned int uiPage);
};

#endif
