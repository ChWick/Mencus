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

#include "HUDMessageBox.hpp"
#include "HUD.hpp"
#include "MessageHandler.hpp"
#include "XMLResources/Manager.hpp"

using namespace CEGUI;

CHUDMessageBox::CHUDMessageBox(const std::string &sID, const CEGUI::String &sTitle, const std::vector<std::string> &vPages)
  : m_sID(sID), m_vPages(vPages) {
  CInputListenerManager::getSingleton().addInputListener(this);

  assert(m_vPages.size() > 0);
  Window *pMainRoot = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  Window *pPane = pMainRoot->createChild("OgreTray/StaticImage");
  m_pRootWindow = pPane;
  pPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pPane->setAlwaysOnTop(true);
  pPane->moveToFront();
  pPane->setProperty("FrameEnabled", "false");
  pPane->setProperty("Image", "OgreTrayImages/Shade");
  
  Window *pWnd = pPane->createChild("OgreTray/Group");
  m_pMessageBox = pWnd;
  // FrameWindow crashes on android, use default window
  //FrameWindow *pWnd = dynamic_cast<FrameWindow*>(m_pRoot->createChild("OgreTray/FrameWindow", label));
  pWnd->setPosition(UVector2(UDim(0.2, 0), UDim(0.2, 0)));
  pWnd->setSize(USize(UDim(0.6, 0), UDim(0.6, 0)));
  pWnd->setText(sTitle);

  Window *pCloseBtn = pWnd->createChild("OgreTray/Button", "CloseButton");
  pCloseBtn->setUserData(dynamic_cast<Window*>(pWnd)); // to be sure that is a Window * ptr
  pCloseBtn->setPosition(UVector2(UDim(0.6, 0), UDim(0.80, 0)));
  pCloseBtn->setSize(USize(UDim(0.35, 0), UDim(0.2, 0)));
  pCloseBtn->setText(XMLResources::GLOBAL.getCEGUIString("close"));
  pCloseBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CHUDMessageBox::onCloseButtonClicked, this));

  Window *pContent = pWnd->createChild("DefaultWindow", "Content");
  pContent->setPosition(UVector2(UDim(0.05, 0), UDim(0.05, 0)));
  pContent->setSize(USize(UDim(0.9, 0), UDim(0.75, 0)));

  //MultiLineEditbox *pTextContainter = dynamic_cast<MultiLineEditbox*>(pContent->createChild("OgreTray/MultiLineEditbox", "Text"));
  // only in static text formatting tags work
  Window *pTextContainter = pContent->createChild("OgreTray/StaticText", "Text");
  pTextContainter->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pTextContainter->setSize(USize(UDim(1, 0), UDim(1, 0)));
  //pTextContainter->setReadOnly(true);
  pTextContainter->setTextParsingEnabled(true);
  // to allow colour formatting tags
  pTextContainter->setProperty("TextColours", "FFFFFFFF");
  pTextContainter->setProperty("VertFormatting", "TopAligned");

  showPage(0);

  pause(PAUSE_MAP_UPDATE);
}
CHUDMessageBox::~CHUDMessageBox() {
  CInputListenerManager::getSingleton().removeInputListener(this);
  unpause(PAUSE_MAP_UPDATE);
  m_pRootWindow->destroy();
}
bool CHUDMessageBox::onCloseButtonClicked(const CEGUI::EventArgs &args) {
  onButtonClicked();
  return true;
}

bool CHUDMessageBox::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_RETURN) {
    return !onButtonClicked();
  }
  return true;
}

bool CHUDMessageBox::onButtonClicked() {
  ++m_uiCurrentPage;
  if (m_uiCurrentPage >= m_vPages.size()) {
    delete this;
    return true;
  }
  else {
    showPage(m_uiCurrentPage);
    return false;
  }
}

void CHUDMessageBox::showPage(unsigned int uiPage) {
  m_uiCurrentPage = uiPage;

  assert(m_vPages.size() > 0);
  assert(m_uiCurrentPage < m_vPages.size());

  CEGUI::Window *pCloseBtn = m_pMessageBox->getChild("CloseButton");
  if (m_uiCurrentPage + 1 >= m_vPages.size()) {
    pCloseBtn->setText(XMLResources::GLOBAL.getCEGUIString("close"));
  }
  else {
    pCloseBtn->setText(XMLResources::GLOBAL.getCEGUIString("next"));
  }
  Window *pTextContainter = m_pMessageBox->getChild("Content")->getChild("Text");
  // default text colour
  pTextContainter->setText("[colour='FF000000']");
  pTextContainter->appendText(reinterpret_cast<const CEGUI::utf8*>(m_vPages[m_uiCurrentPage].c_str()));

  CMessageHandler::getSingleton().addMessage(CMessage(CMessage::MT_MESSAGE_BOX_PAGE_CHANGED).setID(m_sID).setInt(m_uiCurrentPage));
}
