#include "Overlay.hpp"
#include "SocialGaming.hpp"
#include "SocialGamingConnectionInterface.hpp"
#include "MessageTypes.hpp"
#include "MessageHandler.hpp"
#include "Log.hpp"
#include <OgrePlatform.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android/Android.hpp"
#endif

using namespace SocialGaming;
using namespace CEGUI;

COverlay::COverlay(CEGUI::Window *pRoot) 
  : m_pRoot(nullptr),
    m_pSignedInState(nullptr) {
  m_pRoot = pRoot->createChild("DefaultWindow", "SocialGamingOverlayRoot");
  m_pRoot->setInheritsAlpha(false);
  m_pRoot->setAlpha(1);
  m_pRoot->setRiseOnClickEnabled(false);
  m_pRoot->setPosition(UVector2(UDim(0.8f, 0), UDim(0, 0)));
  m_pRoot->setSize(USize(UDim(0.2f, 0), UDim(1, 0)));

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
  
  Window *pRestart = m_pRoot->createChild("OgreTray/Button", "Restart");
  pRestart->setPosition(UVector2(UDim(0, 0), UDim(0, 150)));
  pRestart->setSize(USize(UDim(1, 0), UDim(0, 50)));
  pRestart->setText("Restart");
  pRestart->subscribeEvent(PushButton::EventClicked,
			       Event::Subscriber(&COverlay::onRestart, this));
  

  setSignedIn(CSocialGaming::getSingleton().getConnection()->isSignedIn());

  CMessageHandler::getSingleton().addInjector(this);
}

COverlay::~COverlay() {
  CMessageHandler::getSingleton().removeInjector(this);
  if (m_pRoot) {
    m_pRoot->destroy();
    m_pRoot = nullptr;
  }
}

void COverlay::sendMessageToAll(const CMessage &message) {
  if (message.getType() == CMessage::MT_SOCIAL_GAMING) {
    if (message.getInt() == SIGNED_IN_STATE_CHANGED) {
      //setSignedIn(message.getBool());
    }
  }
}

void COverlay::setSignedIn(bool bSignedIn) {
  m_pSignedInState->setText("Waiting for log in");
  
  if (bSignedIn) {
    m_pSignedInState->setText("Logged in!");
  }
  m_pRoot->getChild("Restart")->setVisible(!bSignedIn);
}

bool COverlay::onShowOverlay(const CEGUI::EventArgs &args) {
  LOGI("before show gaming overlay");
  CSocialGaming::getSingleton().getConnection()->showSocalGamingOverlay();
  LOGI("after show gaming overlay");
  return true;
}
bool COverlay::onRestart(const CEGUI::EventArgs &args) {
  LOGI("before restart");
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  OgreAndroidBridge::callJavaVoid("restartPlugins");
#else
  LOGW("Restart not implemented");
#endif
  LOGI("after restart");
  return true;
}
