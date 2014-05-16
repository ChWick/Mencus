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

#ifndef _REGION_HPP_
#define _REGION_HPP_

#include "Entity.hpp"
#include "DebugDrawer.hpp"
#include "DebugDefines.hpp"

class CRegion : public CEntity {
public:
  CRegion(CMap &map, CEntity *pParent, const tinyxml2::XMLElement *pElem)
    : CEntity(map, pParent, pElem) {
    m_bbRelativeBoundingBox.setSize(m_vSize);
  }
  
  void render(Ogre::Real tpf) {
#ifdef DEBUG_REGION
    CDebugDrawer::getSingleton().draw(getWorldBoundingBox());
#endif
  }

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
    CEntity::writeToXMLElement(pElem, eStyle);
  }
};
#endif
