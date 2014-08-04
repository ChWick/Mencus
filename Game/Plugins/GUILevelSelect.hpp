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

#ifndef _GUI_LEVEL_SELECT_HPP_
#define _GUI_LEVEL_SELECT_HPP_

#include "GUIOverlay.hpp"
#include <CEGUI/CEGUI.h>
#include "LevelList.hpp"

class CGUILevelSelect : public CGUIOverlay {
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pContent;
  CEGUI::Window *m_pChickenButton;

  // level selection windows
  SLevelInfo *m_pLevelInfo;
  CLevelList m_LevelList;
  unsigned int m_uiSelectedLevelID;
public:
  CGUILevelSelect(CEGUI::Window *pParent);
  virtual ~CGUILevelSelect();

protected:
  bool onBackButtonClicked(const CEGUI::EventArgs &args);
  bool onPlayButtonClicked(const CEGUI::EventArgs &args);


  // level selection
  void updateLevelsSelection();
  void selectLevel(unsigned int id);
  bool onLevelButtonClicked(const CEGUI::EventArgs &);
  bool onChickenPressed(const CEGUI::EventArgs&);
};

#endif
