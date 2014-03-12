#include "Object.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Player.hpp"

const Ogre::Vector2 OBJECT_SIZES[CObject::OT_COUNT] = {
  Ogre::Vector2(0.5, 0.5),
  Ogre::Vector2(0.5, 0.5),
  Ogre::Vector2(0.5, 0.5),
  Ogre::Vector2(0.5, 0.25),
  Ogre::Vector2(1, 1),
  Ogre::Vector2(0.5, 1)
};

CObject::CObject(CMap &map,
		 const Ogre::Vector2 &vPosition,
		 EObjectTypes eObjectType)
  : CAnimatedSprite(&map, map.get2dManager(), vPosition, OBJECT_SIZES[eObjectType]),
    m_eObjectType(eObjectType),
    m_Map(map),
    m_bIsPickable(false)
{
  init(0.4, 1);

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
  case OT_TORCH:
    setupAnimation(0, "torch1", 10, CSpriteTexture::MIRROR_NONE, &getTorchTexturePath);
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
      m_Map.getPlayer()->pickobject(m_eObjectType);
    }
  }

  CAnimatedSprite::update(tpf);
}
void CObject::writeToXMLElement(tinyxml2::XMLElement *pElem) {
  CAnimatedSprite::writeToXMLElement(pElem);
  pElem->SetAttribute("type", m_eObjectType);
}
