#include "HUD.hpp"
#include <OgreStringConverter.h>
#include <iostream>

using namespace std;

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
  : m_fTimer(0),
    m_fHP(1),
    m_fMP(1)
{
  m_pHudRoot = pGUIRoot->createChild("DefaultWindow", "HudRoot");
  m_pHudRoot->setInheritsAlpha(false);
  m_pHudRoot->setAlpha(1);
  CEGUI::ImageManager::getSingleton().loadImageset("hud.imageset");
  ImageManager::getSingleton().loadImageset("white.imageset");
  ImageManager::getSingleton().loadImageset("hud_weapons.imageset");

  CEGUI::Window *pMain = m_pHudRoot->createChild("OgreTray/StaticImage", "main");
  pMain->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  pMain->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(1, 0)));
  pMain->setProperty("Image", "hud/main");
  pMain->setProperty("FrameEnabled","False");
  pMain->setProperty("BackgroundEnabled","False");

  m_pFpsText = pMain->createChild("OgreTray/StaticText", "fpstext");
  m_pFpsText->setProperty("TextColours", "FFFFFFFF");
  m_pFpsText->setProperty("FrameEnabled","False");
  m_pFpsText->setProperty("BackgroundEnabled","False");
  m_pFpsText->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
  m_pFpsText->setPosition(CEGUI::UVector2(CEGUI::UDim(0.267, 0), CEGUI::UDim(0.98, 0)));
  m_pFpsText->setText("00");
  m_pFpsText->setFont("dejavusans8");

  m_pHealthBar = pMain->createChild("OgreTray/StaticImage", "healthbar");
  m_pHealthBar->setProperty("ImageColours", "FFFF00FF");
  m_pHealthBar->setProperty("Image", "white/full_image");
  m_pHealthBar->setProperty("FrameEnabled","False");
  m_pHealthBar->setProperty("BackgroundEnabled","True");
  m_pHealthBar->setPosition(UVector2(UDim(0.051757813, 0), UDim(0.026041667, 0)));

  m_pManaBar = pMain->createChild("OgreTray/StaticImage", "manabar");
  m_pManaBar->setProperty("ImageColours", "FFFF00FF");
  m_pManaBar->setProperty("Image", "white/full_image");
  m_pManaBar->setProperty("FrameEnabled", "False");
  m_pManaBar->setProperty("BackgroundEnabled", "True");
  m_pManaBar->setPosition(UVector2(UDim(0.772460938, 0), UDim(0.026041667, 0)));

  m_pWeapon = pMain->createChild("OgreTray/StaticImage", "weapon");
  m_pWeapon->setProperty("FrameEnabled", "False");
  m_pWeapon->setProperty("BackgroundEnabled", "True");
  m_pWeapon->setPosition(UVector2(UDim(0.020507813, 0), UDim(0.936197917, 0)));
  m_pWeapon->setSize(USize(UDim(0.028320313, 0), UDim(0.037760417, 0)));

  m_pHealthPotionsCount = pMain->createChild("OgreTray/StaticText", "HPCount");
  m_pHealthPotionsCount->setProperty("TextColours", "FFFFFFFF");
  m_pHealthPotionsCount->setProperty("FrameEnabled","False");
  m_pHealthPotionsCount->setProperty("BackgroundEnabled","False");
  m_pHealthPotionsCount->setFont("dejavusans8");
  m_pHealthPotionsCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
  m_pHealthPotionsCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.98, 0)));
  m_pHealthPotionsCount->setText("0");

  m_pManaPotionsCount = pMain->createChild("OgreTray/StaticText", "MPCount");
  m_pManaPotionsCount->setProperty("TextColours", "FFFFFFFF");
  m_pManaPotionsCount->setProperty("FrameEnabled","False");
  m_pManaPotionsCount->setProperty("BackgroundEnabled","False");
  m_pManaPotionsCount->setFont("dejavusans8");
  m_pManaPotionsCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
  m_pManaPotionsCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.84, 0), CEGUI::UDim(0.98, 0)));
  m_pManaPotionsCount->setText("0");

  m_pKeyCount = pMain->createChild("OgreTray/StaticText", "KeyCount");
  m_pKeyCount->setProperty("TextColours", "FFFFFFFF");
  m_pKeyCount->setProperty("FrameEnabled","False");
  m_pKeyCount->setProperty("BackgroundEnabled","False");
  m_pKeyCount->setFont("dejavusans8");
  m_pKeyCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
  m_pKeyCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.88, 0), CEGUI::UDim(0.98, 0)));
  m_pKeyCount->setText("0");

  m_pBombCount = pMain->createChild("OgreTray/StaticText", "BombCount");
  m_pBombCount->setProperty("TextColours", "FFFFFFFF");
  m_pBombCount->setProperty("FrameEnabled","False");
  m_pBombCount->setProperty("BackgroundEnabled","False");
  m_pBombCount->setFont("dejavusans8");
  m_pBombCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
  m_pBombCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.92, 0), CEGUI::UDim(0.98, 0)));
  m_pBombCount->setText("0");

  // initialise hp, mp
  setHP(m_fHP);
  setMP(m_fMP);
  setCurrentWeapon(0);

  hide();
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
void CHUD::setHP(Ogre::Real fHP) {
  m_fHP = max(0.0f, min(1.0f, fHP));

  m_pHealthBar->setProperty("ImageColours", getHexValue(getHPColourmap()));
  m_pHealthBar->setSize(USize(UDim(0.204101563 * m_fHP, 0), UDim(0.009114583, 0)));
}
void CHUD::setMP(Ogre::Real fMP) {
  m_fMP = max(0.0f, min(1.0f, fMP));

  m_pManaBar->setProperty("ImageColours", getHexValue(getMPColourmap()));
  m_pManaBar->setSize(USize(UDim(0.204101563 * m_fMP, 0), UDim(0.009114583, 0)));
}
void CHUD::setCurrentWeapon(unsigned int uiWeaponId) {
  if (uiWeaponId == 0) {
    m_pWeapon->setProperty("Image", "hud_weapons/bolt");
  }
  else if (uiWeaponId == 1) {
    m_pWeapon->setProperty("Image", "hud_weapons/bomb");
  }
  else if (uiWeaponId == 2) {
    m_pWeapon->setProperty("Image", "hud_weapons/shield");
  }
}
Ogre::ColourValue CHUD::getHPColourmap() const {
  if (m_fHP == 1.0f) {
    return Ogre::ColourValue(0.01569, 0.5215686, 0.070588235);
  }
  else if (m_fHP > 2.0f/3.0f) {
    Ogre::Real v = 3 * m_fHP-2;
    return Ogre::ColourValue(0.79608 - 0.78039 * v, 0.81569 - 0.29412 * v, 0 + 0.0705882);
  }
  else if (m_fHP > 1.0f/3.0f) {
    Ogre::Real v = 3 * m_fHP - 1;
    return Ogre::ColourValue(0.796078, 0.45098 + 0.3647 * v, 0);
  }
  else {
    Ogre::Real v = 3 * m_fHP;
    return Ogre::ColourValue(0.4902 + 0.20589 * v, 0.003922 + 0.44706 * v, 0);
  }
}
Ogre::ColourValue CHUD::getMPColourmap() const {
  return Ogre::ColourValue(0.423529412 * m_fMP, 0.380392157 + 0.529411765 * m_fMP, 0.337254902 + 0.623529412 * m_fMP);
}
std::string CHUD::getHexValue(const Ogre::ColourValue &c) const {
  std::string out("FFFFFFFF");
  sprintf( &out[0], "FF%02x%02x%02x", min(255, static_cast<int>(c.r * 255)), min(255, static_cast<int>(c.g * 255)), min(255, static_cast<int>(c.b * 255)));
  Ogre::StringUtil::toUpperCase(out);
  return out;
}
