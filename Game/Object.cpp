#include "Object.hpp"
#include "Map.hpp"
#include "Util.hpp"

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
    m_Map(map)
{
  init(1, 1);

  switch (m_eObjectType) {
  case OT_BOMB:
    addTextureToCurrentAnimationSequence(getBombTexture("bomb"));
    break;
  case OT_HEALTH_POTION:
    addTextureToCurrentAnimationSequence(getOtherObjectsTexturePath("health_potion"));
    break;
  case OT_MANA_POTION:
    addTextureToCurrentAnimationSequence(getOtherObjectsTexturePath("mana_potion"));
    break;
  case OT_KEY:
    addTextureToCurrentAnimationSequence(getOtherObjectsTexturePath("key"));
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
  CAnimatedSprite::update(tpf);
}
