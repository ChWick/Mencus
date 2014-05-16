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

#include "Exit.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "DebugDrawer.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

Ogre::String CExit::toString(EExitTypes et) {
  switch (et) {
  case EXIT_REGION:
    return "region";
  case EXIT_ENEMY_DEATH:
    return "enemyDeath";
  }
  throw Ogre::Exception(0, "Unknown exit type", __FILE__);
}
bool CExit::isInExit(CPlayer *pPlayer, CMap *pMap) {
  switch (m_eExitType) {
  case EXIT_REGION:
    if (m_BoundingBox.collidesWith(pPlayer->getWorldBoundingBox()) != CCD_NONE) {
      return true;
    }
    break;
  case EXIT_ENEMY_DEATH:
    if (pMap->getEnemyById(m_sID) == NULL) {
      return true;
    }
    break;
  }
  return false;
}
void CExit::debugDraw() {
  switch (m_eExitType) {
  case EXIT_REGION:
    CDebugDrawer::getSingleton().draw(m_BoundingBox);
    break;
  default:
    break;
  }
}
void CExit::writeToXMLElement(tinyxml2::XMLElement *pElem) const {
  pElem->SetAttribute("type", toString(m_eExitType).c_str());
  SetAttribute(pElem, "pos", m_BoundingBox.getPosition());
  SetAttribute(pElem, "size", m_BoundingBox.getSize());
  pElem->SetAttribute("id", m_sID.c_str());
}
