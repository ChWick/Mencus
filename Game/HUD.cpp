#include "HUD.hpp"
#include <OgreStringConverter.h>

const Ogre::Real HUD_FRAMERATE_UPDATE_TIME = 0.5;

using namespace CEGUI;

template<> CHUD *Ogre::Singleton<CHUD>::msSingleton = 0;

CHUD *CHUD::getSingletonPtr() {
  return msSingleton;
}
CHUD &CHUD::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CHUD::CHUD(CEGUI::Window *pGUIRoot) 
  : m_fTimer(0)
{
  m_pHudRoot = pGUIRoot->createChild("DefaultWindow", "HudRoot");
  m_pHudRoot->setInheritsAlpha(false);
  m_pHudRoot->setAlpha(1);
  CEGUI::ImageManager::getSingleton().loadImageset("hud.imageset");

  CEGUI::Window *pMain = m_pHudRoot->createChild("TaharezLook/StaticImage", "main");
  pMain->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  pMain->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
  pMain->setProperty("Image", "hud/main");
  pMain->setProperty("FrameEnabled","False");
  pMain->setProperty("BackgroundEnabled","False");

  m_pFpsText = pMain->createChild("TaharezLook/StaticText", "fpstext");
  m_pFpsText->setProperty("FrameEnabled","False");
  m_pFpsText->setProperty("BackgroundEnabled","False");
  m_pFpsText->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
  m_pFpsText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.267, 0), CEGUI::UDim(0.98, 0)));
  m_pFpsText->setText("00");

  m_pHealthBar = pMain->createChild("TaharezLook/StaticImage", "healthbar");
  m_pHealthBar->setProperty("BackgroundColours", "tl:FFDFDFDF tr:FFDFDFDF bl:FFDFDFDF br:FFDFDFDF");
  m_pHealthBar->setProperty("FrameEnabled","False");
  m_pHealthBar->setProperty("BackgroundEnabled","True");
  m_pHealthBar->setPosition(UVector2(UDim(0.05, 0), UDim(0.0234, 0)));
  m_pHealthBar->setSize(USize(UDim(0.208, 0), UDim(0.0143, 0)));
}
void CHUD::update(Ogre::Real tpf) {
  m_fTimer += tpf;
  if (m_fTimer > HUD_FRAMERATE_UPDATE_TIME) {
    m_fTimer = 0;
    int iFrames = 1 / tpf;
    if (iFrames > 99) {iFrames = 99;}
    m_pFpsText->setText(Ogre::StringConverter::toString(iFrames, 2).c_str());
  }
}
