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

#ifndef _GUISTATISTICS_HPP_
#define _GUISTATISTICS_HPP_

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "InputListener.hpp"
#include "Statistics.hpp"

class CGUIStatistics : public Ogre::Singleton<CGUIStatistics>, public CInputListener {
private:
  enum EButtons {
    BT_RETRY = 0,
    BT_TO_MENU,
    BT_COUNT
  };
  enum ELabels {
    L_TIME = 0,
    L_HITPOINTS,
    L_MANAPOINTS,
    L_LOST_HITPOINTS,
    L_USED_MANAPOINTS,
    L_COUNT,
  };

  CEGUI::Window *m_pStatisticsRoot;
  CEGUI::Window *m_pButtons[BT_COUNT];
  int m_iSelectedSlot;
public:
  static CGUIStatistics &getSingleton();
  static CGUIStatistics *getSingletonPtr();

  CGUIStatistics(CEGUI::Window *pRoot);
  ~CGUIStatistics();

  void update(Ogre::Real tpf);

  virtual bool keyPressed(const OIS::KeyEvent &arg);

  void show();
  void hide();

  void activateButton(int iBtn);
  void showStatistics(const SStatistics &stats);

  void resize(const CEGUI::String &smallFont, const CEGUI::String &bigfont);

private:
  void changeSelectedSlot(int i);
  void createLabel(int iLabel, CEGUI::Window *pParent, bool bData);
  bool onRetryClicked(const CEGUI::EventArgs&);
  bool onToMenuClicked(const CEGUI::EventArgs&);
};

#endif
