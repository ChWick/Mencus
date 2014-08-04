/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef HUD_HPP
#define HUD_HPP

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "Weapon.hpp"
#include "PauseListener.hpp"
#include <list>

class CGUIToolsMenu;
class CHUDElement;

class CHUD : public Ogre::Singleton<CHUD>, public CPauseListener {
private:
  const bool m_bShotToolIndicators;
  CGUIToolsMenu *m_pGUIToolsMenu;
  CEGUI::Window *m_pHudRoot;
  CEGUI::Window *m_pFpsText;
  CEGUI::Window *m_pTimeText;
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
  std::list<CHUDElement *> m_lElements;
public:
  static CHUD &getSingleton();
  static CHUD *getSingletonPtr();

  CHUD(CEGUI::Window *pGUIRoot, CGUIToolsMenu *pGUIToolsMenu);
  ~CHUD();

  void update(Ogre::Real tpf);

  void setCurrentTime(Ogre::Real fTime);
  void setHP(Ogre::Real fHP);
  void setMP(Ogre::Real fMP);
  void setHealthPotionCount(unsigned int uiCount);
  void setManaPotionCount(unsigned int uiCount);
  void setKeysCount(unsigned int uiCount);
  void setBombCount(unsigned int uiCount);

  void setCurrentWeapon(unsigned int uiWeapon);

  void clear();

  CEGUI::Window *getRoot() {return m_pHudRoot;}

  std::list<CHUDElement *> &getElements() {return m_lElements;}

  void show();
  void hide();


  virtual void mapRenderPauseChanged(bool bPause) {if (bPause) hide(); else show();}
private:
  Ogre::ColourValue getHPColourmap() const;
  Ogre::ColourValue getMPColourmap() const;

  std::string getHexValue(const Ogre::ColourValue &c) const;
};

#endif
