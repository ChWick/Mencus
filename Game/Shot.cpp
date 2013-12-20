#include "Shot.hpp"
#include "Constants.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"

const Ogre::Vector2 SHOT_SIZE[CShot::ST_COUNT] = {
  Ogre::Vector2(0.5, 0.25)
};
const bool SHOT_AFFECTED_BY_GRAVITY[CShot::ST_COUNT] = {
  false
};
const Ogre::Real SHOT_SPEED[CShot::ST_COUNT] = {
  5.0
};

CShot::CShot(CMap *pMap,
	     Ogre2dManager *pSpriteManager,
	     const Ogre::Vector2 &vPosition,
	     EShotTypes eShotType,
	     EShotDirections eShotDirection)
  :
  CAnimatedSprite(pMap,
		  pSpriteManager,
		  vPosition,
		  SHOT_SIZE[eShotType]),
  m_pMap(pMap),
  m_eShotType(eShotType),
  m_bAffectedByGravity(SHOT_AFFECTED_BY_GRAVITY[eShotType]),
  m_vSpeed(Ogre::Vector2::ZERO),
  m_eShotDirection(eShotDirection) {

  init(1, 1);

  CSpriteTexture::EMirrorTypes eMirrorType = CSpriteTexture::MIRROR_NONE;
  if (eShotDirection == SD_LEFT) {eMirrorType = CSpriteTexture::MIRROR_Y;}

  setDefaultGetPath(&getPlayerTexturePath);
  if (m_eShotType == ST_BOLT) {
    setupAnimation(0, "bolt", 2, eMirrorType);
  }
}
void CShot::launch(const Ogre::Vector2 &vInitialSpeed) {
  m_vSpeed = vInitialSpeed * SHOT_SPEED[m_eShotType];
}
void CShot::update(Ogre::Real tpf) {
  // change the position and speed
  if (m_bAffectedByGravity) {
    m_vSpeed.y += c_fGravity * tpf;
  }

  m_vPosition += m_vSpeed * tpf;

  // check for collisions
  if (m_pMap->hitsTile(CTile::TF_UNPASSABLE, getWorldBoundingBox())) {
    m_pMap->destroyShot(this);
    return;
  }
  if (m_pMap->outOfMap(getWorldBoundingBox())) {
    m_pMap->destroyShot(this);
    return;
  }

  CAnimatedSprite::update(tpf);
}
		  
