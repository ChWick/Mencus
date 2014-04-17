#include "HUDMessageBox.hpp"
#include "HUD.hpp"
#include "MessageHandler.hpp"
#include "XMLResources/Manager.hpp"

using namespace CEGUI;

CHUDMessageBox::CHUDMessageBox(const std::string &sID, const CEGUI::String &sTitle, const std::vector<std::string> &vPages)
  : m_sID(sID), m_vPages(vPages) {
  assert(m_vPages.size() > 0);
  Window *pWnd = CHUD::getSingleton().getRoot()->createChild("OgreTray/Group");
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

  MultiLineEditbox *pTextContainter = dynamic_cast<MultiLineEditbox*>(pContent->createChild("OgreTray/MultiLineEditbox", "Text"));
  pTextContainter->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pTextContainter->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pTextContainter->setReadOnly(true);
  
  showPage(0);
  
  pause(PAUSE_MAP_UPDATE);
}
CHUDMessageBox::~CHUDMessageBox() {
  unpause(PAUSE_MAP_UPDATE);
  m_pMessageBox->destroy();
}
bool CHUDMessageBox::onCloseButtonClicked(const CEGUI::EventArgs &args) {
  ++m_uiCurrentPage;
  if (m_uiCurrentPage >= m_vPages.size()) {
    delete this;
  }
  else {
    showPage(m_uiCurrentPage);
  }
  return true;
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
  pTextContainter->setText(reinterpret_cast<const CEGUI::utf8*>(m_vPages[m_uiCurrentPage].c_str()));

  CMessageHandler::getSingleton().addMessage(CMessage(CMessage::MT_MESSAGE_BOX_PAGE_CHANGED).setID(m_sID).setInt(m_uiCurrentPage));
}
