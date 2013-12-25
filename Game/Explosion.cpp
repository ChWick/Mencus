#include "Explosion.hpp"
#include "Map.hpp"
#include "Util.hpp"

const Ogre::Vector2 EXPLOSION_SIZES[CExplosion::ET_COUNT] = {
  Ogre::Vector2(0.5, 0.25),
  Ogre::Vector2(2, 2),
  Ogre::Vector2(1, 1)
};
const Ogre::Vector2 EXPLOSION_RELATIVE_OFFSET[CExplosion::ET_COUNT] = {
  EXPLOSION_SIZES[CExplosion::ET_BOLT] / 2,
  EXPLOSION_SIZES[CExplosion::ET_BOMB] / 2,
  EXPLOSION_SIZES[CExplosion::ET_GREEN_MONSTER] / 2
};

CExplosion::CExplosion(CMap *pMap, const Ogre::Vector2 &vCenter, EExplosionTypes eExplosionType)
  :
  CAnimatedSprite(pMap,
		  pMap->get2dManager(),
		  vCenter - EXPLOSION_RELATIVE_OFFSET[eExplosionType],
		  EXPLOSION_SIZES[eExplosionType]),
  m_pMap(pMap),
  m_eExplosionType(eExplosionType) {
  
  init(1, 1);

  CSpriteTexture::EMirrorTypes eMirrorType = CSpriteTexture::MIRROR_NONE;

  if (m_eExplosionType == ET_BOLT) {
    setupAnimation(0, "bolt_explosion", 5, eMirrorType, &getPlayerTexturePath);
  }
  else if (m_eExplosionType == ET_BOMB) {
    setupAnimation(0, "bomb_explosion", 7, eMirrorType, &getBombTexture);
  }
  else if (m_eExplosionType == ET_GREEN_MONSTER) {
    setupAnimation(0, "explosion", 7, eMirrorType, &getEnemyTexturePath<1>);
  }
}
void CExplosion::update(Ogre::Real tpf) {
  CAnimatedSprite::update(tpf);
}
void CExplosion::animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText) {
  // at the end of the sequence destroy the explosion
  if (uiNewText == 0 && uiOldText == m_AnimationSequences[m_uiCurrentAnimationSequence].size() - 1) {
    m_pMap->destroyExplosion(this);
  }
}
