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

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "AnimatedSprite.hpp"

class CMap;

class CObject : public CAnimatedSprite {
public:
  enum EObjectTypes {
    OT_BOMB,
    OT_HEALTH_POTION,
    OT_MANA_POTION,
    OT_KEY,
    OT_SCRATCH,
    OT_TORCH,
    OT_FLAG,

    OT_COUNT
  };

  static Ogre::String getPreviewImageName(int iObjectType);
private:
  bool m_bIsPickable;
public:
  CObject(CMap &map,
	  const std::string &sID,
	  CEntity *pParent,
	  const Ogre::Vector2 &vPosition,
	  EObjectTypes eObjectType);

  CObject(CMap &map,
	  CEntity *pParent,
	  const tinyxml2::XMLElement *pElement);

  void update(Ogre::Real tpf);

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;

private:
  void constructor_impl();
};


#endif
