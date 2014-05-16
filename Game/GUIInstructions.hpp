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

#ifndef INSTRUCTIONS_GUI_HPP
#define INSTRUCTIONS_GUI_HPP

#include <CEGUI/CEGUI.h>
#include <OgreString.h>
#include <OgreSingleton.h>
#include "InputListener.hpp"
#include "ScreenplayListener.hpp"

class CGUIInstructions : public Ogre::Singleton<CGUIInstructions>, public CInputListener {
private:
  CEGUI::Window *m_pInstructionsRoot;
  CEGUI::Window *m_pText;
  CEGUI::Window *m_pPressKeyText;
  CScreenplayListener *m_pScreenplayListener;
public:
  static CGUIInstructions &getSingleton();
  static CGUIInstructions *getSingletonPtr();


  CGUIInstructions(CEGUI::Window *pGUIRoot);
  ~CGUIInstructions();

  void setText(const CEGUI::String &sText) {m_pText->setText(sText);}

  void show() {m_pInstructionsRoot->setVisible(true); setInputListenerEnabled(true);}
  void hide() {m_pInstructionsRoot->setVisible(false); setInputListenerEnabled(false);}

  void setScreenplayListener(CScreenplayListener *pListener) {m_pScreenplayListener = pListener;}

  virtual bool keyPressed( const OIS::KeyEvent &arg );
  bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  bool touchPressed(const OIS::MultiTouchEvent& evt);
};
#endif // INSTRUCTIONS_GUI_HPP
