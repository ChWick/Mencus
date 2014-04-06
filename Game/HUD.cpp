#include "HUD.hpp"
#include "GUIInput.hpp"
#include <OgreStringConverter.h>
#include <iostream>
#include "HUDElement.hpp"

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

CHUD::CHUD(CEGUI::Window *pGUIRoot, CGUIInput *pGUIInput)
  : m_bShotToolIndicators(pGUIInput == NULL), 
    m_pGUIInput(pGUIInput),
    m_fTimer(0),
    m_fHP(1),
    m_fMP(1)
{
  m_pHudRoot = pGUIRoot->createChild("DefaultWindow", "HudRoot");
  m_pHudRoot->setInheritsAlpha(false);
  m_pHudRoot->setAlpha(1);
  m_pHudRoot->setRiseOnClickEnabled(false);
  CEGUI::ImageManager::getSingleton().loadImageset("hud.imageset");
  ImageManager::getSingleton().loadImageset("white.imageset");
  ImageManager::getSingleton().loadImageset("hud_weapons.imageset");

  CEGUI::Window *pMain = m_pHudRoot->createChild("OgreTray/StaticText", "main");
  pMain->setRiseOnClickEnabled(false);
  pMain->setProperty("BackgroundEnabled", "false");
  pMain->setProperty("FrameEnabled", "false");
  pMain->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pMain->setSize(USize(UDim(1, 0), UDim(1, 0)));
  //pMain->setText("test");
  //pMain->setAlpha(0.5);

  CEGUI::Window *pTopBar = pMain->createChild("OgreTray/StaticImage", "top_bar");
  pTopBar->setPosition(CEGUI::UVector2(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0)));
  pTopBar->setSize(CEGUI::USize(CEGUI::UDim(1, 0), CEGUI::UDim(0.1667f, 0)));
  pTopBar->setProperty("Image", "hud/top_bar");
  pTopBar->setProperty("FrameEnabled","False");
  pTopBar->setProperty("BackgroundEnabled","False");
  pTopBar->setRiseOnClickEnabled(false);


  Window *pBottomBar = pMain->createChild("OgreTray/StaticImage", "bottom_bar");
  pBottomBar->setSize(USize(UDim(0.28125, 0), UDim(0.05859375, 0)));
  pBottomBar->setPosition(UVector2(UDim(0.359375, 0), UDim(0.94140625, 0)));
  pBottomBar->setProperty("Image", "hud/bottom_bar");
  pBottomBar->setProperty("FrameEnabled","False");
  pBottomBar->setProperty("BackgroundEnabled","False");
  pBottomBar->setRiseOnClickEnabled(false);

  m_pFpsText = pBottomBar->createChild("OgreTray/StaticText", "fpstext");
  m_pFpsText->setProperty("TextColours", "FFFFFFFF");
  m_pFpsText->setProperty("FrameEnabled","False");
  m_pFpsText->setProperty("BackgroundEnabled","False");
  m_pFpsText->setSize(USize(UDim(0.2, 0), UDim(0.3, 0)));
  m_pFpsText->setPosition(UVector2(UDim(0.605, 0), UDim(0.66, 0)));
  m_pFpsText->setText("00");
  m_pFpsText->setFont("dejavusans8");
  m_pFpsText->setRiseOnClickEnabled(false);

  m_pTimeText = pBottomBar->createChild("OgreTray/StaticText", "timetext");
  m_pTimeText->setProperty("TextColours", "FFFFFFFF");
  m_pTimeText->setProperty("FrameEnabled","False");
  m_pTimeText->setProperty("BackgroundEnabled","False");
  m_pTimeText->setSize(USize(UDim(0.2, 0), UDim(0.3, 0)));
  m_pTimeText->setPosition(UVector2(UDim(0.25, 0), UDim(0.66, 0)));
  m_pTimeText->setText("0:00:00");
  m_pTimeText->setFont("dejavusans8");
  m_pTimeText->setRiseOnClickEnabled(false);

  m_pHealthBar = pMain->createChild("OgreTray/StaticImage", "healthbar");
  m_pHealthBar->setProperty("ImageColours", "FFFF00FF");
  m_pHealthBar->setProperty("Image", "white/full_image");
  m_pHealthBar->setProperty("FrameEnabled","False");
  m_pHealthBar->setProperty("BackgroundEnabled","True");
  m_pHealthBar->setPosition(UVector2(UDim(0.051757813, 0), UDim(0.026041667, 0)));
  m_pHealthBar->setRiseOnClickEnabled(false);

  m_pManaBar = pMain->createChild("OgreTray/StaticImage", "manabar");
  m_pManaBar->setProperty("ImageColours", "FFFF00FF");
  m_pManaBar->setProperty("Image", "white/full_image");
  m_pManaBar->setProperty("FrameEnabled", "False");
  m_pManaBar->setProperty("BackgroundEnabled", "True");
  m_pManaBar->setPosition(UVector2(UDim(0.772460938, 0), UDim(0.026041667, 0)));
  m_pManaBar->setRiseOnClickEnabled(false);

  if (m_bShotToolIndicators) {
    Window *pWeaponContainer = pMain->createChild("OgreTray/StaticImage", "weaponContainer");
    pWeaponContainer->setProperty("FrameEnabled", "False");
    pWeaponContainer->setProperty("BackgroundEnabled", "False");
    pWeaponContainer->setProperty("Image", "hud/weapon_indicator");
    pWeaponContainer->setPosition(UVector2(UDim(0, 0), UDim(0.908854, 0)));
    pWeaponContainer->setSize(USize(UDim(0.0683594, 0), UDim(0.091145833, 0)));
    
    
    m_pWeapon = pMain->createChild("OgreTray/StaticImage", "weapon");
    m_pWeapon->setProperty("FrameEnabled", "False");
    m_pWeapon->setProperty("BackgroundEnabled", "True");
    m_pWeapon->setPosition(UVector2(UDim(0.020507813, 0), UDim(0.936197917, 0)));
    m_pWeapon->setSize(USize(UDim(0.028320313, 0), UDim(0.037760417, 0)));

    Window *pImg = pMain->createChild("OgreTray/StaticImage", "HPImg");
    pImg->setProperty("FrameEnabled","False");
    pImg->setProperty("BackgroundEnabled","False");
    pImg->setSize(CEGUI::USize(UDim(0, 32), UDim(0, 32)));
    pImg->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8, -16), CEGUI::UDim(0.98, -24)));
    pImg->setProperty("Image", "hud_weapons/health_potion");

    m_pHealthPotionsCount = pMain->createChild("OgreTray/StaticText", "HPCount");
    m_pHealthPotionsCount->setProperty("TextColours", "FFFFFFFF");
    m_pHealthPotionsCount->setProperty("FrameEnabled","False");
    m_pHealthPotionsCount->setProperty("BackgroundEnabled","False");
    m_pHealthPotionsCount->setFont("dejavusans8");
    m_pHealthPotionsCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
    m_pHealthPotionsCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8, 0), CEGUI::UDim(0.98, -8)));
    m_pHealthPotionsCount->setText("0");

    pImg = pMain->createChild("OgreTray/StaticImage", "MPImg");
    pImg->setProperty("FrameEnabled","False");
    pImg->setProperty("BackgroundEnabled","False");
    pImg->setSize(CEGUI::USize(UDim(0, 32), UDim(0, 32)));
    pImg->setPosition(CEGUI::UVector2(CEGUI::UDim(0.84, -16), CEGUI::UDim(0.98, -24)));
    pImg->setProperty("Image", "hud_weapons/mana_potion");

    m_pManaPotionsCount = pMain->createChild("OgreTray/StaticText", "MPCount");
    m_pManaPotionsCount->setProperty("TextColours", "FFFFFFFF");
    m_pManaPotionsCount->setProperty("FrameEnabled","False");
    m_pManaPotionsCount->setProperty("BackgroundEnabled","False");
    m_pManaPotionsCount->setFont("dejavusans8");
    m_pManaPotionsCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
    m_pManaPotionsCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.84, 0), CEGUI::UDim(0.98, -8)));
    m_pManaPotionsCount->setText("0");

    pImg = pMain->createChild("OgreTray/StaticImage", "KeyImg");
    pImg->setProperty("FrameEnabled","False");
    pImg->setProperty("BackgroundEnabled","False");
    pImg->setSize(CEGUI::USize(UDim(0, 32), UDim(0, 32)));
    pImg->setPosition(CEGUI::UVector2(CEGUI::UDim(0.88, -16), CEGUI::UDim(0.98, -20)));
    pImg->setProperty("Image", "hud_weapons/keys");

    m_pKeyCount = pMain->createChild("OgreTray/StaticText", "KeyCount");
    m_pKeyCount->setProperty("TextColours", "FFFFFFFF");
    m_pKeyCount->setProperty("FrameEnabled","False");
    m_pKeyCount->setProperty("BackgroundEnabled","False");
    m_pKeyCount->setFont("dejavusans8");
    m_pKeyCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
    m_pKeyCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.88, 0), CEGUI::UDim(0.98, -8)));
    m_pKeyCount->setText("0");

    pImg = pMain->createChild("OgreTray/StaticImage", "BombImg");
    pImg->setProperty("FrameEnabled","False");
    pImg->setProperty("BackgroundEnabled","False");
    pImg->setSize(CEGUI::USize(UDim(0, 32), UDim(0, 32)));
    pImg->setPosition(CEGUI::UVector2(CEGUI::UDim(0.92, -16), CEGUI::UDim(0.98, -24)));
    pImg->setProperty("Image", "hud_weapons/bomb");

    m_pBombCount = pMain->createChild("OgreTray/StaticText", "BombCount");
    m_pBombCount->setProperty("TextColours", "FFFFFFFF");
    m_pBombCount->setProperty("FrameEnabled","False");
    m_pBombCount->setProperty("BackgroundEnabled","False");
    m_pBombCount->setFont("dejavusans8");
    m_pBombCount->setSize(CEGUI::USize(UDim(0.1, 0), UDim(0.02,0)));
    m_pBombCount->setPosition(CEGUI::UVector2(CEGUI::UDim(0.92, 0), CEGUI::UDim(0.98, -8)));
    m_pBombCount->setText("0");
  }

  // initialise hp, mp
  setHP(m_fHP);
  setMP(m_fMP);
  setCurrentWeapon(Weapon::W_BOLT);

  hide();
}
CHUD::~CHUD() {
  clear();
}
void CHUD::clear() {
  while (m_lElements.size() > 0) {
    delete m_lElements.front();
  }
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
void CHUD::setHealthPotionCount(unsigned int uiCount) {
  if (m_bShotToolIndicators) {
    m_pHealthPotionsCount->setText(CEGUI::PropertyHelper<unsigned int>::toString(uiCount));
  }
  if (m_pGUIInput) {
    m_pGUIInput->setItemCount(Weapon::I_HEALTH_POTION, uiCount);
  }
}
void CHUD::setManaPotionCount(unsigned int uiCount) {
  if (m_bShotToolIndicators) {
    m_pManaPotionsCount->setText(CEGUI::PropertyHelper<unsigned int>::toString(uiCount));
  }
  if (m_pGUIInput) {
    m_pGUIInput->setItemCount(Weapon::I_MANA_POTION, uiCount);
  }
}
void CHUD::setKeysCount(unsigned int uiCount) {
  if (m_bShotToolIndicators) {
    m_pKeyCount->setText(CEGUI::PropertyHelper<unsigned int>::toString(uiCount));
  }
  if (m_pGUIInput) {
    m_pGUIInput->setItemCount(Weapon::I_KEY, uiCount);
  }
}
void CHUD::setBombCount(unsigned int uiCount) {
  if (m_bShotToolIndicators) {
    m_pBombCount->setText(CEGUI::PropertyHelper<unsigned int>::toString(uiCount));
  }
  if (m_pGUIInput) {
    m_pGUIInput->setItemCount(Weapon::I_BOMB, uiCount);
  }
}
void CHUD::setCurrentWeapon(unsigned int uiWeapon) {
  if (m_bShotToolIndicators) {
    m_pWeapon->setProperty("Image", Weapon::getPicture(uiWeapon));
  }
  if (m_pGUIInput) {
    m_pGUIInput->setCurrentWeapon(uiWeapon);
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
void CHUD::setCurrentTime(Ogre::Real fTime) {
  int iHours = static_cast<int>(fTime / 3600);
  int iMinutes = static_cast<int>((fTime - iHours * 3600) / 60);
  int iSeconds = static_cast<int>(fTime - iHours * 3600 - iMinutes * 60);

  m_pTimeText->setText((Ogre::StringConverter::toString(iHours, 1) + ":"
			+ Ogre::StringConverter::toString(iMinutes, 2, '0') + ":"
			+ Ogre::StringConverter::toString(iSeconds, 2, '0')).c_str());
}
void CHUD::show() {
  m_pHudRoot->setVisible(true);
  if (m_pGUIInput) {
    m_pGUIInput->show();
  }
}
void CHUD::hide() {
  m_pHudRoot->setVisible(false);
  if (m_pGUIInput) {
    m_pGUIInput->hide();
  }
}
