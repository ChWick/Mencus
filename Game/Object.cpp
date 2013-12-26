#include "Object.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Player.hpp"

const Ogre::Vector2 OBJECT_SIZES[CObject::OT_COUNT] = {
  Ogre::Vector2(0.5, 0.5),
  Ogre::Vector2(1, 1),
  Ogre::Vector2(1, 1)
};

CObject::CObject(CMap &map,
		 const Ogre::Vector2 &vPosition,
		 EObjectTypes eObjectType)
  : CAnimatedSprite(&map, map.get2dManager(), vPosition, OBJECT_SIZES[eObjectType]),
    m_eObjectType(eObjectType),
    m_Map(map),
    m_bIsPickable(false)
{
  init(1, 1);

  switch (m_eObjectType) {
  case OT_BOMB:
    addTextureToCurrentAnimationSequence(getBombTexture("bomb"));
    m_bIsPickable = true;
    break;
  case OT_HEALTH_POTION:
    addTextureToCurrentAnimationSequence(getOtherObjectsTexturePath("health_potion"));
    m_bIsPickable = true;
    break;
  case OT_MANA_POTION:
    addTextureToCurrentAnimationSequence(getOtherObjectsTexturePath("mana_potion"));
    m_bIsPickable = true;
    break;
  case OT_KEY:
    addTextureToCurrentAnimationSequence(getOtherObjectsTexturePath("key"));
    m_bIsPickable = true;
    break;
  case OT_SCRATCH:
    addTextureToCurrentAnimationSequence(getOtherObjectsTexturePath("scratch"));
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Object type '" + Ogre::StringConverter::toString(m_eObjectType) + "' is unknown!", __FILE__);
    break;
  }
}

void CObject::update(Ogre::Real tpf) {
  if (m_bIsPickable) {
    if (m_Map.getPlayer()->getWorldBoundingBox().contains(getCenter()) ) {
      m_Map.destroyObject(this);
    }
  }

  CAnimatedSprite::update(tpf);
}