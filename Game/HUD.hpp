#ifndef HUD_HPP
#define HUD_HPP

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "Weapon.hpp"

class CGUIInput;

class CHUD : public Ogre::Singleton<CHUD> {
private:
  const bool m_bShotToolIndicators;
  CGUIInput *m_pGUIInput;
  CEGUI::Window *m_pHudRoot;
  CEGUI::Window *m_pFpsText;
  CEGUI::Window *m_pHealthBar;
  CEGUI::Window *m_pManaBar;
  CEGUI::Window *m_pWeapon;
  CEGUI::Window *m_pHealthPotionsCount;
  CEGUI::Window *m_pManaPotionsCount;
  CEGUI::Window *m_pKeyCount;
  CEGUI::Window *m_pBombCount;
  Ogre::Real m_fTimer;
  Ogre::Real m_fHP;		//!< Hp of the player as value [0,1]
  Ogre::Real m_fMP;		//!< Mp of the player as value [0,1]
public:
  static CHUD &getSingleton();
  static CHUD *getSingletonPtr();

  CHUD(CEGUI::Window *pGUIRoot, CGUIInput *pGUIInput);

  void update(Ogre::Real tpf);

  void setHP(Ogre::Real fHP);
  void setMP(Ogre::Real fMP);
  void setHealthPotionCount(unsigned int uiCount);
  void setManaPotionCount(unsigned int uiCount);
  void setKeysCount(unsigned int uiCount);
  void setBombCount(unsigned int uiCount);

  void setCurrentWeapon(unsigned int uiWeapon);

  void show();
  void hide();
private:
  Ogre::ColourValue getHPColourmap() const;
  Ogre::ColourValue getMPColourmap() const;

  std::string getHexValue(const Ogre::ColourValue &c) const;
};

#endif
