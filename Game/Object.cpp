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

#include "Object.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Player.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

const Ogre::Vector2 OBJECT_SIZES[CObject::OT_COUNT] = {
  Ogre::Vector2(0.5, 0.5),     // Bomb
  Ogre::Vector2(0.5, 0.5),     // Health potion
  Ogre::Vector2(0.5, 0.5),     // Mana potion
  Ogre::Vector2(0.9, 0.45),    // Key
  Ogre::Vector2(1, 1),	       // Scratch
  Ogre::Vector2(0.5, 1),       // Torch 
  Ogre::Vector2(1, 2)	       // Flag
};

CObject::CObject(CMap &map,
		 const std::string &sID,
		 CEntity *pParent,
		 const Ogre::Vector2 &vPosition,
		 EObjectTypes eObjectType)
  : CAnimatedSprite(map,
		    sID,
		    pParent,
		    &map,
		    map.get2dManager(),
		    vPosition,
		    OBJECT_SIZES[eObjectType]),
    m_bIsPickable(false)
{
  setType(eObjectType);
  constructor_impl();
}
CObject::CObject(CMap &map,
		 CEntity *pParent,
		 const tinyxml2::XMLElement *pElement) 
  : CAnimatedSprite(map,
		    pParent,
		    &map,
		    map.get2dManager(),
		    pElement,
		    OBJECT_SIZES[IntAttribute(pElement, "type")]) ,
    m_bIsPickable(false) {
  constructor_impl();
}
void CObject::constructor_impl() {
  init(0.4, 1);

  switch (m_uiType) {
  case OT_BOMB:
    addTextureToAnimationSequence(0, getBombTexture("bomb"));
    m_bIsPickable = true;
    break;
  case OT_HEALTH_POTION:
    addTextureToAnimationSequence(0, getOtherObjectsTexturePath("health_potion"));
    m_bIsPickable = true;
    break;
  case OT_MANA_POTION:
    addTextureToAnimationSequence(0, getOtherObjectsTexturePath("mana_potion"));
    m_bIsPickable = true;
    break;
  case OT_KEY:
    addTextureToAnimationSequence(0, getOtherObjectsTexturePath("key"));
    m_bIsPickable = true;
    m_bbRelativeBoundingBox.setSize(m_vSize * 0.7);
    m_bbRelativeBoundingBox.setPosition(m_vSize * 0.15);
    break;
  case OT_SCRATCH:
    addTextureToAnimationSequence(0, getOtherObjectsTexturePath("scratch"));
    break;
  case OT_TORCH:
    m_fAnimationSpeed = 0.12;
    setupAnimation(0, "torch1", 10, CSpriteTexture::MIRROR_NONE, &getTorchTexturePath);
    break;
  case OT_FLAG:
    m_fAnimationSpeed = 0.12;
    setupAnimation(0, "flag", 12, CSpriteTexture::MIRROR_NONE, &getFlagTexturePath);
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Object type '" + Ogre::StringConverter::toString(m_uiType) + "' is unknown!", __FILE__);
    break;
  }

  setSleeping(getGravity() == 0);
}
Ogre::String CObject::getPreviewImageName(int iObjectType) {
  switch (iObjectType) {
  case OT_BOMB:
    return getBombTexture("bomb");
  case OT_HEALTH_POTION:
    return getOtherObjectsTexturePath("health_potion");
  case OT_MANA_POTION:
    return getOtherObjectsTexturePath("mana_potion");
  case OT_KEY:
    return getOtherObjectsTexturePath("key");
  case OT_SCRATCH:
    return getOtherObjectsTexturePath("scratch");
  case OT_TORCH:
    return getTorchTexturePath("torch1_1");
  case OT_FLAG:
    return getFlagTexturePath("flag_1");
  }
  return "pink.png";
}

void CObject::update(Ogre::Real tpf) {
  if (m_bIsPickable) {
    if (m_Map.getPlayer()->getWorldBoundingBox().contains(getCenter()) ) {
      destroy();
      m_Map.getPlayer()->pickobject(m_uiType);
    }
  }

  CAnimatedSprite::update(tpf);
}
void CObject::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CAnimatedSprite::writeToXMLElement(pElem, eStyle);
}
