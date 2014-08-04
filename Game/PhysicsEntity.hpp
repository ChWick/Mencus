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

#ifndef PHYSICSENTITY_HPP
#define PHYSICSENTITY_HPP

#include <Entity.hpp>


class CPhysicsEntity : public CEntity {
protected:
  bool m_bSleeping;
  Ogre::Real m_fGravity;
  Ogre::Real m_fBounce;
  Ogre::Real m_fFriction;
  Ogre::Vector2 m_vSpeed;
  bool m_bOnGround;
public:
  CPhysicsEntity(CMap &map, const std::string &sID, CEntity *pParent);
  CPhysicsEntity(CMap &map,
		 CEntity *pParent,
		 const tinyxml2::XMLElement *pElem,
		 const Ogre::Vector2 &vDefaultPosition = Ogre::Vector2::ZERO,
		 const Ogre::Vector2 &vDefaultSize = Ogre::Vector2::UNIT_SCALE,
		 const Ogre::Vector2 &vDefaultScale = Ogre::Vector2::UNIT_SCALE);
  CPhysicsEntity(const CPhysicsEntity &src);
  virtual ~CPhysicsEntity();

  virtual void update(Ogre::Real tpf);

  void setSleeping(bool bSleep) {m_bSleeping = bSleep;}
  bool isSleeping() const {return m_bSleeping;}

  void setGravity(Ogre::Real fGravity) {m_fGravity = fGravity;}
  Ogre::Real getGravity() const {return m_fGravity;}

  void setSpeed(const Ogre::Vector2 &vSpeed) {m_vSpeed = vSpeed;}
  const Ogre::Vector2 &getSpeed() const {return m_vSpeed;}

  void setBounce(Ogre::Real fBounce) {m_fBounce = fBounce;}
  Ogre::Real getBounce() const {return m_fBounce;}

  void setFriction(Ogre::Real fFriction) {m_fFriction = fFriction;}
  Ogre::Real getFriction() const {return m_fFriction;}

  bool isOnGround() const {return m_bOnGround;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;
protected:
  virtual bool handleHorizontalTileCollision(unsigned int uiCCD, Ogre::Real tpf);
  virtual bool handleVerticalTileCollision(unsigned int uiCCD, Ogre::Real tpf);
  virtual void handleOutOfMap() {}
private:
};

#endif // PHYSICSENTITY_HPP
