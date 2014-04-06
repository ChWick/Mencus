#include "HUDMessageBox.hpp"
#include "HUD.hpp"

using namespace CEGUI;

CHUDMessageBox::CHUDMessageBox(const char *pTitle, const char *pText) {
  Window *pWnd = CHUD::getSingleton().getRoot()->createChild("OgreTray/Group");
  m_pMessageBox = pWnd;
  // FrameWindow crashes on android, use default window
  //FrameWindow *pWnd = dynamic_cast<FrameWindow*>(m_pRoot->createChild("OgreTray/FrameWindow", label));
  pWnd->setPosition(UVector2(UDim(0.2, 0), UDim(0.2, 0)));
  pWnd->setSize(USize(UDim(0.6, 0), UDim(0.6, 0)));
  pWnd->setText(pTitle);

  Window *pCloseBtn = pWnd->createChild("OgreTray/Button", "CloseButton");
  pCloseBtn->setUserData(dynamic_cast<Window*>(pWnd)); // to be sure that is a Window * ptr
  pCloseBtn->setPosition(UVector2(UDim(0.6, 0), UDim(0.85, 0)));
  pCloseBtn->setSize(USize(UDim(0.35, 0), UDim(0.1, 0)));
  pCloseBtn->setText("Close");
  pCloseBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CHUDMessageBox::onCloseButtonClicked, this));

  Window *pContent = pWnd->createChild("DefaultWindow", "Content");
  pContent->setPosition(UVector2(UDim(0.05, 0), UDim(0.05, 0)));
  pContent->setSize(USize(UDim(0.9, 0), UDim(0.75, 0)));

  Window *pTextContainter = pContent->createChild("OgreTray/StaticText", "Text");
  pTextContainter->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pTextContainter->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pTextContainter->setText(pText);
}
CHUDMessageBox::~CHUDMessageBox() {
  m_pMessageBox->destroy();
}
bool CHUDMessageBox::onCloseButtonClicked(const CEGUI::EventArgs &args) {
  delete this;
  return true;
}
