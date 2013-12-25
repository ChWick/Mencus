#ifndef HUD_HPP
#define HUD_HPP

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
class CHUD : public Ogre::Singleton<CHUD> {
private:
  CEGUI::Window *m_pHudRoot;
  CEGUI::Window *m_pFpsText;
  CEGUI::Window *m_pHealthBar;
  CEGUI::Window *m_pManaBar;
  CEGUI::Window *m_pWeapon;
  Ogre::Real m_fTimer;
  Ogre::Real m_fHP;		//!< Hp of the player as value [0,1]
  Ogre::Real m_fMP;		//!< Mp of the player as value [0,1]
public:
  static CHUD &getSingleton();
  static CHUD *getSingletonPtr();

  CHUD(CEGUI::Window *pGUIRoot);

  void update(Ogre::Real tpf);

  void setHP(Ogre::Real fHP);
  void setMP(Ogre::Real fMP);

  void setCurrentWeapon(unsigned int uiWeaponId);
private:
  Ogre::ColourValue getHPColourmap() const;
  Ogre::ColourValue getMPColourmap() const;

  std::string getHexValue(const Ogre::ColourValue &c) const;
};

#endif
