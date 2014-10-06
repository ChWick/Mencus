#include "Overlay.hpp"
#include "SocialGaming.hpp"
#include "SocialGamingConnectionInterface.hpp"
#include "MessageTypes.hpp"
#include "MessageHandler.hpp"
#include "Log.hpp"
#include "Settings.hpp"

using namespace SocialGaming;
using namespace CEGUI;

COverlay::COverlay(CEGUI::Window *pRoot) 
  : m_pRoot(nullptr),
    m_pSignedInState(nullptr) {
  m_pShowButton = pRoot->createChild("OgreTray/ImageButton", "SocialGamingOverlayShowButton");
  m_pShowButton->setPosition(UVector2(UDim(1, -100), UDim(0, 0)));
  m_pShowButton->setSize(USize(UDim(0, 100), UDim(0, 100)));
  m_pShowButton->setProperty("Image",
			     "social_gaming_logos/"
			     + CSocialGaming::getSingleton().
			     getConnection()->getLogoImagesetLabel());
  m_pShowButton->subscribeEvent(PushButton::EventClicked,
				Event::Subscriber(&COverlay::onShowSocialGamingSideBar, this));

  m_pRoot = pRoot->createChild("OgreTray/Group", "SocialGamingOverlayRoot");
  m_pRoot->setInheritsAlpha(false);
  m_pRoot->setAlpha(1);
  m_pRoot->setRiseOnClickEnabled(false);
  m_pRoot->setPosition(UVector2(UDim(0.7f, 0), UDim(0, 0)));
  m_pRoot->setSize(USize(UDim(0.3f, 0), UDim(1, 0)));
  m_pRoot->setText(CSocialGaming::getSingleton().getConnection()->getName());
  m_pRoot->setVisible(false);

  Window *pStatus = m_pRoot->createChild("OgreTray/StaticText", "Status");
  pStatus->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pStatus->setSize(USize(UDim(1, 0), UDim(0, 100)));
  m_pSignedInState = pStatus;

  Window *pShowOverlay = m_pRoot->createChild("OgreTray/Button", "ShowOverlay");
  pShowOverlay->setPosition(UVector2(UDim(0, 0), UDim(0, 100)));
  pShowOverlay->setSize(USize(UDim(1, 0), UDim(0, 50)));
  pShowOverlay->setText("Show overlay");
  pShowOverlay->subscribeEvent(PushButton::EventClicked,
			       Event::Subscriber(&COverlay::onShowOverlay, this));
  
  Window *pRestart = m_pRoot->createChild("OgreTray/Button", "Login");
  pRestart->setPosition(UVector2(UDim(0, 0), UDim(0, 100)));
  pRestart->setSize(USize(UDim(1, 0), UDim(0, 50)));
  pRestart->setText("Log in");
  pRestart->subscribeEvent(PushButton::EventClicked,
			   Event::Subscriber(&COverlay::onRestart, this));

  Window *pLoginOnStart = m_pRoot->createChild("OgreTray/Checkbox", "LoginOnStart");
  pLoginOnStart->setPosition(UVector2(UDim(1, -50), UDim(0, 150)));
  pLoginOnStart->setSize(USize(UDim(0, 50), UDim(0, 50)));
  pLoginOnStart->subscribeEvent(ToggleButton::EventSelectStateChanged,
				Event::Subscriber(&COverlay::onLoginOnStartToggled,
						  this));

  Window *pLoginOnStartLabel = m_pRoot->createChild("OgreTray/Label", "LoginOnStartLabel");
  pLoginOnStartLabel->setPosition(UVector2(UDim(0, 0), UDim(0, 150)));
  pLoginOnStartLabel->setSize(USize(UDim(1, -50), UDim(0, 50)));
  pLoginOnStartLabel->setText("Login on start");


  Window *pHideButton = m_pRoot->createChild("OgreTray/Button", "Hide");
  pHideButton->setPosition(UVector2(UDim(0, 0), UDim(1, -50)));
  pHideButton->setSize(USize(UDim(1, 0), UDim(0, 50)));
  pHideButton->setText("Hide");
  pHideButton->subscribeEvent(PushButton::EventClicked,
			       Event::Subscriber(&COverlay::onHideSocialGamingSideBar, this));
  

  setSignedIn(CSocialGaming::getSingleton().getConnection()->isSignedIn());
  dynamic_cast<ToggleButton*>(pLoginOnStart)
    ->setSelected(CSettings::getSingleton()
		  .getSocialGamingSettings().m_bLoginOnStart);						  
  CMessageHandler::getSingleton().addInjector(this);

  changeTouchButtonSize(100);
}

COverlay::~COverlay() {
  CMessageHandler::getSingleton().removeInjector(this);
  if (m_pRoot) {
    m_pRoot->destroy();
    m_pRoot = nullptr;
  }
}

void COverlay::changeTouchButtonSize(float fSize) {
  float fSmallButtonSize = fSize * 0.5f;
  m_pShowButton->setPosition(UVector2(UDim(1, -fSize), UDim(0, 0)));
  m_pShowButton->setSize(USize(UDim(0, fSize), UDim(0, fSize)));

  m_pRoot->getChild("Status")->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  m_pRoot->getChild("Status")->setSize(USize(UDim(1, 0), UDim(0, fSmallButtonSize)));

  m_pRoot->getChild("ShowOverlay")->setPosition(UVector2(UDim(0, 0), UDim(0, fSmallButtonSize)));
  m_pRoot->getChild("ShowOverlay")->setSize(USize(UDim(1, 0), UDim(0, fSmallButtonSize)));

  m_pRoot->getChild("Login")->setPosition(UVector2(UDim(0, 0), UDim(0, fSmallButtonSize)));
  m_pRoot->getChild("Login")->setSize(USize(UDim(1, 0), UDim(0, fSmallButtonSize)));

  m_pRoot->getChild("LoginOnStart")->setPosition(UVector2(UDim(1, -fSmallButtonSize), UDim(0, 0 * fSize + 2 * fSmallButtonSize)));
  m_pRoot->getChild("LoginOnStart")->setSize(USize(UDim(0, fSmallButtonSize), UDim(0, fSmallButtonSize)));

  
  m_pRoot->getChild("LoginOnStartLabel")->setPosition(UVector2(UDim(0, 0), UDim(0, 0 * fSize + 2 * fSmallButtonSize)));
  m_pRoot->getChild("LoginOnStartLabel")->setSize(USize(UDim(1, -fSmallButtonSize), UDim(0, fSmallButtonSize)));
}

void COverlay::sendMessageToAll(const CMessage &message) {
  if (message.getType() == CMessage::MT_SOCIAL_GAMING) {
    if (message.getInt() == SIGNED_IN_STATE_CHANGED) {
      setSignedIn(message.getBool());
    }
  }
}

void COverlay::setSignedIn(bool bSignedIn) {
  if (bSignedIn) {
    m_pSignedInState->setText("Logged in!");
  }
  else {
    m_pSignedInState->setText("Not connected.");
  }
  m_pRoot->getChild("Login")->setVisible(!bSignedIn);
  m_pRoot->getChild("ShowOverlay")->setVisible(bSignedIn);
}

bool COverlay::onShowSocialGamingSideBar(const CEGUI::EventArgs &args) {
  m_pRoot->setVisible(true);
  m_pShowButton->setVisible(false);
  return true;
}

bool COverlay::onHideSocialGamingSideBar(const CEGUI::EventArgs &args) {
  m_pRoot->setVisible(false);
  m_pShowButton->setVisible(true);
  return true;
}

bool COverlay::onShowOverlay(const CEGUI::EventArgs &args) {
  LOGI("before show gaming overlay");
  CSocialGaming::getSingleton().getConnection()->showSocalGamingOverlay();
  LOGI("after show gaming overlay");
  return true;
}
bool COverlay::onRestart(const CEGUI::EventArgs &args) {
  LOGI("before restart");
  CSocialGaming::getSingleton().init();
  LOGI("after restart");
  return true;
}

bool COverlay::onLoginOnStartToggled(const CEGUI::EventArgs &args) {
  const ToggleButton *pToggleButton = dynamic_cast<const ToggleButton*>(dynamic_cast<const WindowEventArgs&>(args).window);
  CSettings::getSingleton().getSocialGamingSettings().m_bLoginOnStart = 
    pToggleButton->isSelected();
  return true;
}
