#include "HUD.hpp"

template<> CHUD *Ogre::Singleton<CHUD>::msSingleton = 0;

CHUD *CHUD::getSingletonPtr() {
  return msSingleton;
}
CHUD &CHUD::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CHUD::CHUD(CEGUI::Window *pGUIRoot) 
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
}
void CHUD::update(Ogre::Real tpf) {
}
