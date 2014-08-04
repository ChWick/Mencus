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

#ifndef GUIINPUT_HPP
#define GUIINPUT_HPP

#include <CEGUI/CEGUI.h>
#include <OgreVector2.h>
#include "MessageInjector.hpp"

class CGUIInput
  : public CMessageInjector {
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
  static EButtonTypes parseButtonType(const std::string &s);
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pDirectionButtonContainer;
  CEGUI::Window *m_pControlButtonContainer;
  CEGUI::Window *m_pButtons[BT_COUNT];
  Ogre::Vector2 m_vButtonOrigins[BT_COUNT];
  bool m_bButtonPressed[BT_COUNT];

  float m_fButtonSize;

  // for making buttons blink
  float m_fTimer;
  std::list<CEGUI::Window *> m_lBlinkingButtons;
public:
  CGUIInput(CEGUI::Window *pGUIRoot);
  ~CGUIInput();

  void update(float tpf);

  void show();
  void hide();

  void windowResized();

  void buttonSizeChanged(float fSize);

  void sendMessageToAll(const CMessage &message);
private:
  
  void updateInput();
  void checkForButtonPress(const Ogre::Vector2 &vPos);
  int getButtonAtPos(const Ogre::Vector2 &vPos);

  CEGUI::Window *createButton(int bt);
};

#endif
