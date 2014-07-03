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

#ifndef _GUI_SETTINGS_WINDOW_HPP_
#define _GUI_SETTINGS_WINDOW_HPP_

#include <CEGUI/CEGUI.h>
#include "Plugins/GUIOverlay.hpp"

class CGUISettingsWindow : public CGUIOverlay {
protected:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pContent;
public:
  CGUISettingsWindow(CEGUI::Window *pParentRoot, const CEGUI::String &sTitle);
  virtual ~CGUISettingsWindow();

protected:
  bool onCloseButtonClicked(const CEGUI::EventArgs &args);
};

#endif
