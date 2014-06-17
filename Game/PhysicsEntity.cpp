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

#include "PhysicsEntity.hpp"
#include "Constants.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

CPhysicsEntity::CPhysicsEntity(CMap &map,
			       const std::string &sID,
			       CEntity *pParent)
  : CEntity(map, sID, pParent),
    m_bSleeping(true),
    m_fGravity(c_fGravity),
    m_fBounce(0.5),
    m_fFriction(10),
    m_vSpeed(Ogre::Vector2::ZERO) {
}

CPhysicsEntity::CPhysicsEntity(CMap &map,
			       CEntity *pParent,
			       const tinyxml2::XMLElement *pElem,
			       const Ogre::Vector2 &vDefaultPosition,
			       const Ogre::Vector2 &vDefaultSize,
			       const Ogre::Vector2 &vDefaultScale)
  : CEntity(map, pParent, pElem, vDefaultPosition, vDefaultSize, vDefaultScale),
    m_bSleeping(BoolAttribute(pElem, "phy_sleeping", true)),
    m_fGravity(RealAttribute(pElem, "phy_gravity", c_fGravity)),
    m_fBounce(RealAttribute(pElem, "phy_bounce", 0.5)),
    m_fFriction(RealAttribute(pElem, "phy_friction", 10)),
    m_vSpeed(Vector2Attribute(pElem, "phy_speed_", Ogre::Vector2::ZERO))  {
}

CPhysicsEntity::CPhysicsEntity(const CPhysicsEntity &src)
  : CEntity(src),
    m_bSleeping(src.m_bSleeping),
    m_fGravity(src.m_fGravity),
    m_fBounce(src.m_fBounce),
    m_fFriction(src.m_fFriction),
    m_vSpeed(src.m_vSpeed) {

}

CPhysicsEntity::~CPhysicsEntity() {
}

void CPhysicsEntity::update(Ogre::Real tpf) {
  CEntity::update(tpf);
  
  if (m_bSleeping) {return;}

  // gravity
  m_vSpeed.y += m_fGravity * tpf;

  // delta step
  Ogre::Vector2 vDelta(m_vSpeed * tpf);
  CBoundingBox2d bbOverlap;
  // horizontal movement
  m_vPosition.x += vDelta.x;
  unsigned int uiCCD = m_Map.hitsTile(CTile::TF_UNPASSABLE,
				      getWorldBoundingBox(),
				      &bbOverlap);
  if (uiCCD > 0 && handleHorizontalTileCollision(uiCCD, tpf)) {
    // undo step
    m_vPosition.x -= vDelta.x;
  }

  // vertical motion
  m_vPosition.y += vDelta.y;
  uiCCD = m_Map.hitsTile(CTile::TF_UNPASSABLE,
			 getWorldBoundingBox(),
			 &bbOverlap);
  m_bOnGround = false;
  if (uiCCD > 0 && handleVerticalTileCollision(uiCCD, tpf)) {
    // undo
    m_vPosition.y -= vDelta.y;
  }

  if (m_Map.outOfMap(getWorldBoundingBox())) {
    handleOutOfMap();
  }
  if (m_bOnGround) {
    m_vSpeed.x -= m_vSpeed.x * m_fFriction * tpf;
  }
}

bool CPhysicsEntity::handleHorizontalTileCollision(unsigned int uiCCD, Ogre::Real tpf) {
  if (uiCCD & CCD_RIGHT && m_vSpeed.x > 0) {
    m_vSpeed.x *= -m_fBounce;
    return true;
  } else if (uiCCD & CCD_LEFT && m_vSpeed.x < 0) {
    m_vSpeed.x *= -m_fBounce;
    return true;
  }

  return false;
}

bool CPhysicsEntity::handleVerticalTileCollision(unsigned int uiCCD, Ogre::Real tpf) {
  if (uiCCD & CCD_BOTTOM && m_vSpeed.y < 0) {
    m_vSpeed.y *= -0.5;
    m_bOnGround = true;
    return true;
  } else if (uiCCD & CCD_TOP && m_vSpeed.y > 0) {
    m_vSpeed.y *= -0.5;
    return true;
  }

  return false;
}

void CPhysicsEntity::writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const {
SetAttribute(pElement, "phy_sleeping", (m_bSleeping) ? "true" : "false");
SetAttribute(pElement, "phy_gravity", m_fGravity);
SetAttribute(pElement, "phy_bounce", m_fBounce);
SetAttribute(pElement, "phy_friction", m_fFriction);
SetAttribute(pElement, "phy_speed_", m_vSpeed);

CEntity::writeToXMLElement(pElement, eStyle);
}
