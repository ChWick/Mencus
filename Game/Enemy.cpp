#include "Enemy.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Tile.hpp"
#include "DebugDrawer.hpp"
#include "Explosion.hpp"
#include "Player.hpp"

const Ogre::Vector2 ENEMY_SIZE[CEnemy::ET_COUNT] = {
  Ogre::Vector2(1, 1)
};
const Ogre::Real ENEMY_SPEED[CEnemy::ET_COUNT] = {
  3.0
};

CEnemy::CEnemy(CMap &map,
	       const Ogre::Vector2 &vPosition,
	       EEnemyTypes eEnemyType,
	       Ogre::Real fDirection,
	       Ogre::Real fHitpoints)
  : CAnimatedSprite(&map, map.get2dManager(), vPosition, ENEMY_SIZE[eEnemyType]),
    CHitableObject(fHitpoints),
    m_Map(map),
    m_eEnemyType(eEnemyType),
    m_fSpeed(fDirection * ENEMY_SPEED[eEnemyType]) {
  setup();
}
void CEnemy::update(Ogre::Real tpf) {
  CAnimatedSprite::update(tpf);
  
  unsigned int ccd = getWorldBoundingBox().collidesWith(m_Map.getPlayer()->getWorldBoundingBox());
  if (ccd != CCD_NONE) {
    changeCurrentAnimationSequence((ccd & CCD_LEFT) ? AS_ATTACK_LEFT : AS_ATTACK_RIGHT); 
  }
  else {
    m_vPosition.x += m_fSpeed * tpf;

    Ogre::Real fPenetration = 0;
    if (m_fSpeed < 0) {
      fPenetration = m_Map.hitsTile(CCD_LEFT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
    }
    else {
      fPenetration = m_Map.hitsTile(CCD_RIGHT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
    }

    if (fPenetration != 0) {
      m_vPosition.x -= fPenetration;
      m_fSpeed *= -1;
    }
  
    if (m_Map.collidesWithMapMargin(getWorldBoundingBox())) {
      m_vPosition.x -= m_fSpeed * tpf;
      m_fSpeed *= -1;
    }
    if (m_fSpeed < 0) {
      changeCurrentAnimationSequence(AS_WALK_LEFT);
    }
    else {
      changeCurrentAnimationSequence(AS_WALK_RIGHT);
    }
  }

#ifdef DEBUG_CHARACTER_BOUNDING_BOXES
  CDebugDrawer::getSingleton().draw(getWorldBoundingBox());
#endif
}
void CEnemy::setup() {
  switch (m_eEnemyType) {
  case ET_GREEN_MONSTER:
    init(ENEMY_SPEED[m_eEnemyType], 4);
    setDefaultGetPath(&getEnemyTexturePath<1>);
    setupAnimation(AS_WALK_LEFT, "walk_right", 4, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", 4);
    setCurrentAnimationSequence(AS_ATTACK_LEFT);
    {
      CSpriteTexture &attackLeft = addTextureToCurrentAnimationSequence(getEnemyTexturePath<1>("attack_right", 1));
      attackLeft.mirror(CSpriteTexture::MIRROR_Y);
      attackLeft.setSpriteOffset(Ogre::Vector2(-1, 0));
      attackLeft.setSpriteScale(Ogre::Vector2(2, 1));
      addTextureToCurrentAnimationSequence(getEnemyTexturePath<1>("walk_right", 1)).mirror(CSpriteTexture::MIRROR_Y);
    }
    {
      setCurrentAnimationSequence(AS_ATTACK_RIGHT);
      CSpriteTexture &attackRight = addTextureToCurrentAnimationSequence(getEnemyTexturePath<1>("attack_right", 1));
      attackRight.setSpriteScale(Ogre::Vector2(2, 1));
      addTextureToCurrentAnimationSequence(getEnemyTexturePath<1>("walk_right", 1));
    }
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Enemy type '" + Ogre::StringConverter::toString(m_eEnemyType) + "' is unknown", __FILE__);
  }
}
void CEnemy::killedByDamageCallback() {
  m_Map.destroyEnemy(this);
  m_Map.addExplosion(new CExplosion(&m_Map, getCenter(), static_cast<CExplosion::EExplosionTypes>(m_eEnemyType + CExplosion::ET_GREEN_MONSTER)));
}

void CEnemy::animationTextureChangedCallback(unsigned int uiOldText, unsigned uiNewText) {
  if (uiOldText == m_AnimationSequences[m_uiCurrentAnimationSequence].size() - 1 && uiNewText == 0) {
    if (m_uiCurrentAnimationSequence == AS_ATTACK_RIGHT || m_uiCurrentAnimationSequence == AS_ATTACK_LEFT) {
      m_Map.getPlayer()->takeDamage(0.1);
    }
  }
}
