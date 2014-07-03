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

#include "GUISettingsWindow.hpp"
#include "XMLResources/Manager.hpp"
#include "GUIManager.hpp"

using namespace CEGUI;

CGUISettingsWindow::CGUISettingsWindow(Window *pParent, const CEGUI::String &sTitle) {
  Window *pPane = pParent->createChild("OgreTray/StaticImage");
  m_pRoot = pPane;
  pPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pPane->setAlwaysOnTop(true);
  pPane->moveToFront();
  pPane->setProperty("FrameEnabled", "false");
  pPane->setProperty("Image", "OgreTrayImages/Shade");
  
  Window *pWnd = pPane->createChild("OgreTray/Group");
  pWnd->setPosition(UVector2(UDim(0.1, 0), UDim(0.1, 0)));
  pWnd->setSize(USize(UDim(0.8, 0), UDim(0.8, 0)));
  pWnd->setText(sTitle);

  Window *pCloseBtn = pWnd->createChild("OgreTray/Button", "CloseButton");
  pCloseBtn->setUserData(dynamic_cast<Window*>(pWnd)); // to be sure that is a Window * ptr
  pCloseBtn->setPosition(UVector2(UDim(0.6, 0), UDim(0.80, 0)));
  pCloseBtn->setSize(USize(UDim(0.35, 0), UDim(0.2, 0)));
  pCloseBtn->setText(XMLResources::GLOBAL.getCEGUIString("close"));
  pCloseBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGUISettingsWindow::onCloseButtonClicked, this));

  Window *pContent = pWnd->createChild("DefaultWindow", "Content");
  m_pContent = pContent;
  pContent->setPosition(UVector2(UDim(0.05, 0), UDim(0.05, 0)));
  pContent->setSize(USize(UDim(0.9, 0), UDim(0.75, 0)));
}
CGUISettingsWindow::~CGUISettingsWindow() {
  m_pRoot->destroy();
}
bool CGUISettingsWindow::onCloseButtonClicked(const CEGUI::EventArgs &args) {
  CGUIManager::getSingleton().destroyGUIOverlay(this);
  return true;
}
