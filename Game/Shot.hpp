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

#ifndef SHOT_HPP
#define SHOT_HPP

#include "AnimatedSprite.hpp"

class CMap;

class CShot : public CAnimatedSprite {
public:
  enum EShotStates {
    SS_NONE,
    SS_LAUNCHED,
    SS_ENEMY_CAUGHT,
  };
  enum EShotAnimations {
    SA_DEFAULT = 0,
    SA_LAUNCHED,

    SA_COUNT
  };
  enum EShotDirections {
    SD_LEFT,
    SD_RIGHT,
  };
  enum EShotTypes {
    ST_BOLT,
    ST_BOMB,
    ST_SKULL,
    ST_COLUMN,

    ST_COUNT,
  };
  enum EDamages {
    DMG_PLAYER = 1,
    DMG_ENEMY  = 2,
    DMG_ALL    = 3,
  };

  static const Ogre::Real SHOT_DAMAGE[ST_COUNT];
private:
  const bool m_bAffectedByGravity;
  Ogre::Vector2 m_vSpeed;
  const EShotDirections m_eShotDirection;
  Ogre::Real m_fTimer;
  EShotStates m_eState;
  unsigned int m_uiDamages;
  CEntity *m_pCatchedEnemy;
  std::string m_sCatchedEnemyID;
public:
  CShot(CMap &map,
	const Ogre::Vector2 &vCenter,
	EShotTypes eShotType,
	EShotDirections eShotDirection,
	unsigned int uiDmg = DMG_ALL);
  CShot(CMap &map,
	const tinyxml2::XMLElement *pElement);

  void init();
  //! Lanches a shot with the given speed
  /** This speed will be multiplied with the default speed of the shot
   *  \param[in] vInitialSpeed The initial speed or direction vector
   *  \param[in] uiAnimationSequence New animation sequence on launch, if SA_COUNT class will handle on its own
   */
  void launch(const Ogre::Vector2 &vInitialSpeed, unsigned int uiNewAnimationSequence = SA_COUNT);

  void update(Ogre::Real tpf);

  void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;
protected:
  virtual void handleMessage(const CMessage &message);
private:
  void constructor_impl();
  void hit();
};

#endif
