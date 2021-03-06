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

#ifndef GUI_GAME_OVER
#define GUI_GAME_OVER

#include <CEGUI/CEGUI.h>
#include <OgreString.h>
#include <OgreSingleton.h>
#include "InputListener.hpp"

class CGUIGameOver : public Ogre::Singleton<CGUIGameOver>, public CInputListener {
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pRetryText;
  CEGUI::Window *m_pExitText;

  int m_iSelectedText;
public:
  static CGUIGameOver &getSingleton();
  static CGUIGameOver *getSingletonPtr();


  CGUIGameOver(CEGUI::Window *pGUIRoot);
  ~CGUIGameOver();

  void update(Ogre::Real tpf);

  void show() {m_pRoot->setVisible(true); setInputListenerEnabled(true); m_iSelectedText = 0;}
  void hide() {m_pRoot->setVisible(false); setInputListenerEnabled(false);}

  virtual bool keyPressed( const OIS::KeyEvent &arg );
};

#endif // GUI_GAME_OVER
