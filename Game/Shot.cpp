#include "Shot.hpp"
#include "Constants.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Explosion.hpp"
#include "Enemy.hpp"

const Ogre::Vector2 SHOT_SIZE[CShot::ST_COUNT] = {
  Ogre::Vector2(0.5, 0.25),
  Ogre::Vector2(0.5, 0.5)
};
const bool SHOT_AFFECTED_BY_GRAVITY[CShot::ST_COUNT] = {
  false,
  true
};
const Ogre::Real SHOT_SPEED[CShot::ST_COUNT] = {
  10.0,
  1.0
};
const Ogre::Real CShot::SHOT_DAMAGE[CShot::ST_COUNT] = {
  1,
  2
};
const Ogre::Real BOMB_EXPLOSION_TIME = 5;
const Ogre::Real BOMB_EXPLOSION_RADIUS = 1.75;

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
  m_eShotDirection(eShotDirection),
  m_bLaunched(false) {

  CSpriteTexture::EMirrorTypes eMirrorType = CSpriteTexture::MIRROR_NONE;
  if (eShotDirection == SD_LEFT) {
    eMirrorType = CSpriteTexture::MIRROR_Y;
  }

  setDefaultGetPath(&getPlayerTexturePath);
  if (m_eShotType == ST_BOLT) {
    init(1, 1);
    setupAnimation(SA_DEFAULT, "bolt", 2, eMirrorType);
  } else if (m_eShotType == ST_BOMB) {
    init(1, 2);
    setDefaultGetPath(&getBombTexture);
    setupAnimation(SA_DEFAULT, "bomb", -1, eMirrorType);
    setupAnimation(SA_LAUNCHED, "bomb_on", 6, eMirrorType);
  }

  changeCurrentAnimationSequence(SA_DEFAULT);
}
void CShot::launch(const Ogre::Vector2 &vInitialSpeed, unsigned int uiNewAnimationSequence) {
  m_fTimer = 0;
  m_bLaunched = true;
  m_vSpeed = vInitialSpeed * SHOT_SPEED[m_eShotType];
  if (uiNewAnimationSequence == SA_COUNT) {
    switch (m_eShotType) {
    case ST_BOLT:
      uiNewAnimationSequence = SA_DEFAULT;
      break;
    case ST_BOMB:
      uiNewAnimationSequence = SA_LAUNCHED;
      break;
    default:
      break;
    }
  }
  changeCurrentAnimationSequence(uiNewAnimationSequence);
}
void CShot::update(Ogre::Real tpf) {
  if (m_bLaunched) {
    // change the position and speed
    if (m_bAffectedByGravity) {
      m_vSpeed.y += c_fGravity * tpf;
    }

    if (m_eShotType == ST_BOLT) {
      m_vPosition += m_vSpeed * tpf;

      // check for collisions
      if (m_pMap->hitsTile(CTile::TF_UNPASSABLE, getWorldBoundingBox())) {
        // create explosion
        m_pMap->addExplosion(new CExplosion(m_pMap, getCenter(), CExplosion::ET_BOLT));
        m_pMap->destroyShot(this);
      }
      if (m_pMap->outOfMap(getWorldBoundingBox())) {
        // just destroy it
        m_pMap->destroyShot(this);
      }

      hitEnemy();
    } else if (m_eShotType == ST_BOMB) {
      m_fTimer += tpf;
      if (m_fTimer > BOMB_EXPLOSION_TIME) {
        m_pMap->createExplosion(getCenter(), BOMB_EXPLOSION_RADIUS);
        m_pMap->destroyShot(this);
      } else {
        bool bOnGround = false;
        CBoundingBox2d bbOverlap;
        unsigned int uiCCD = m_pMap->hitsTile(CTile::TF_UNPASSABLE,
                                              getWorldBoundingBox(),
                                              &bbOverlap);

        m_vPosition.x += m_vSpeed.x * tpf;
        if (uiCCD & CCD_RIGHT && m_vSpeed.x > 0) {
          m_vPosition.x -= m_vSpeed.x * tpf;
          m_vSpeed.x *= -0.5;
        } else if (uiCCD & CCD_LEFT && m_vSpeed.x < 0) {
          m_vPosition.x -= m_vSpeed.x * tpf;
          m_vSpeed.x *= -0.5;
        }

        m_vPosition.y += m_vSpeed.y * tpf;
        uiCCD = m_pMap->hitsTile(CTile::TF_UNPASSABLE,
                                 getWorldBoundingBox(),
                                 &bbOverlap);
        if (uiCCD & CCD_BOTTOM && m_vSpeed.y < 0) {
          m_vPosition.y -= m_vSpeed.y * tpf;
          m_vSpeed.y *= -0.5;
          bOnGround = true;
        } else if (uiCCD & CCD_TOP && m_vSpeed.y > 0) {
          m_vPosition.y -= m_vSpeed.y * tpf;
          m_vSpeed.y *= -0.5;
        }

        if (bOnGround) {
          m_vSpeed.x -= m_vSpeed.x * 2 * tpf;
        }
      }
    }
  }

  CAnimatedSprite::update(tpf);
}
void CShot::hitEnemy() {
  for (auto *pEnemy : m_pMap->getEnemies()) {
    if (pEnemy->getWorldBoundingBox().collidesWith(getWorldBoundingBox())) {
      pEnemy->takeDamage(SHOT_DAMAGE[m_eShotType]);
      m_pMap->destroyShot(this);
      return;
    }
  }
}
