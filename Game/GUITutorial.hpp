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

#ifndef _GUI_TUTORIAL_HPP_
#define _GUI_TUTORIAL_HPP_

#include <CEGUI/CEGUI.h>
#include <OgreSingleton.h>
#include "PauseCaller.hpp"

class CGUITutorial 
  : public Ogre::Singleton<CGUITutorial>,
    public CPauseCaller {
  CEGUI::Window *m_pRoot;
public:
  static CGUITutorial &getSingleton();
  static CGUITutorial *getSingletonPtr();

  CGUITutorial(CEGUI::Window *pRoot);

  void showMoveInformationWindow();
  void showSwitchInformationWindow();
  void showEnemyInformationWindow();
  void showSmallEnemyInformationWindow();

private:
  void showSimpleTextMessageBox(const CEGUI::String &label, const CEGUI::String &content);
  CEGUI::Window *createMessageBox(const CEGUI::String &label);

  bool onCloseButtonClicked(const CEGUI::EventArgs &);
};


#endif
