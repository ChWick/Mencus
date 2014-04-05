#include "Object.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Player.hpp"

const Ogre::Vector2 OBJECT_SIZES[CObject::OT_COUNT] = {
  Ogre::Vector2(0.5, 0.5),     // Bomb
  Ogre::Vector2(0.5, 0.5),     // Health potion
  Ogre::Vector2(0.5, 0.5),     // Mana potion
  Ogre::Vector2(0.7, 0.35),    // Key
  Ogre::Vector2(1, 1),	       // Scratch
  Ogre::Vector2(0.5, 1)	       // Torch 
};

CObject::CObject(CMap &map,
		 const Ogre::Vector2 &vPosition,
		 EObjectTypes eObjectType)
  : CAnimatedSprite(map,
		    &map,
		    map.get2dManager(),
		    vPosition,
		    OBJECT_SIZES[eObjectType]),
    m_eObjectType(eObjectType),
    m_Map(map),
    m_bIsPickable(false)
{
  init(0.4, 1);

  switch (m_eObjectType) {
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
    break;
  case OT_SCRATCH:
    addTextureToAnimationSequence(0, getOtherObjectsTexturePath("scratch"));
    break;
  case OT_TORCH:
    setupAnimation(0, "torch1", 10, CSpriteTexture::MIRROR_NONE, &getTorchTexturePath);
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Object type '" + Ogre::StringConverter::toString(m_eObjectType) + "' is unknown!", __FILE__);
    break;
  }
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
  }
  return "pink.png";
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
void CObject::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CAnimatedSprite::writeToXMLElement(pElem, eStyle);
  pElem->SetAttribute("type", m_eObjectType);
}
