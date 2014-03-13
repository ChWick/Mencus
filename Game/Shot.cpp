#include "Shot.hpp"
#include "Constants.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Explosion.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

const Ogre::Real COLUMN_CATCH_DURATION = 3.0f;

const Ogre::Vector2 SHOT_SIZE[CShot::ST_COUNT] = {
  Ogre::Vector2(0.5, 0.25),
  Ogre::Vector2(0.5, 0.5),
  Ogre::Vector2(0.5, 0.5),
  Ogre::Vector2(1, 2)
};
const bool SHOT_AFFECTED_BY_GRAVITY[CShot::ST_COUNT] = {
  false,
  true,
  false,
  false
};
const Ogre::Real SHOT_SPEED[CShot::ST_COUNT] = {
  10.0,
  1.0,
  7.0,
  5.0
};
const Ogre::Real CShot::SHOT_DAMAGE[CShot::ST_COUNT] = {
  1,
  2,
  0.5,
  0.25
};
const Ogre::Real BOMB_EXPLOSION_TIME = 5;
const Ogre::Real BOMB_EXPLOSION_RADIUS = 1.75;

CShot::CShot(CMap *pMap,
             Ogre2dManager *pSpriteManager,
             const Ogre::Vector2 &vPosition,
             EShotTypes eShotType,
             EShotDirections eShotDirection,
             unsigned int uiDmg)
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
  m_bLaunched(false),
  m_uiDamages(uiDmg),
  m_pCatchedEnemy(NULL) {
  constructor_impl();
}
CShot::CShot(CMap *pMap,
	     const tinyxml2::XMLElement *pElement) 
  : CAnimatedSprite(pMap,
		   pMap->get2dManager(),
		   pElement,
		   SHOT_SIZE[EnumAttribute<EShotTypes>(pElement, "type", ST_COUNT, true)]),
    m_pMap(pMap),
    m_eShotType(EnumAttribute<EShotTypes>(pElement, "type", ST_COUNT, true)),
    m_bAffectedByGravity(BoolAttribute(pElement,
				       "affected_by_gravity",
				       SHOT_AFFECTED_BY_GRAVITY[EnumAttribute<EShotTypes>(pElement, "type", ST_COUNT, true)])),
    m_vSpeed(Vector2Attribute(pElement, "speed", Ogre::Vector2::ZERO)),
  m_eShotDirection(EnumAttribute<EShotDirections>(pElement, "direction", SD_RIGHT, true)),
  m_bLaunched(BoolAttribute(pElement, "launched", false)),
  m_uiDamages(IntAttribute(pElement, "damages", DMG_ALL, true)),
  m_pCatchedEnemy(pMap->getEnemyById(Attribute(pElement, "catched_enemy"))) {
  constructor_impl();
}
void CShot::constructor_impl() {
  CSpriteTexture::EMirrorTypes eMirrorType = CSpriteTexture::MIRROR_NONE;
  if (m_eShotDirection == SD_LEFT) {
    eMirrorType = CSpriteTexture::MIRROR_Y;
  }

  if (m_eShotType == ST_BOLT) {
    init(1, 1);
    setupAnimation(SA_DEFAULT, "bolt", 2, eMirrorType, &getBoltTexture);
  }
  else if (m_eShotType == ST_BOMB) {
    init(1, 2);
    setDefaultGetPath(&getBombTexture);
    setupAnimation(SA_DEFAULT, "bomb", -1, eMirrorType);
    setupAnimation(SA_LAUNCHED, "bomb_on", 6, eMirrorType);
  }
  else if (m_eShotType == ST_SKULL) {
    init(1, 1);
    setupAnimation(SA_DEFAULT, "skull", 1, eMirrorType, &getSkullTexture);
  }
  else if (m_eShotType == ST_COLUMN) {
    init(1, 1);
    setupAnimation(SA_DEFAULT, "column", 2, eMirrorType, &getColumnTexture);
    m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.4, 0));
    m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.2, 1.6));
  }

  changeCurrentAnimationSequence(SA_DEFAULT);
}
void CShot::launch(const Ogre::Vector2 &vInitialSpeed, unsigned int uiNewAnimationSequence) {
  m_fTimer = 0;
  m_bLaunched = true;
  m_vSpeed = vInitialSpeed * SHOT_SPEED[m_eShotType];
  if (uiNewAnimationSequence == SA_COUNT) {
    switch (m_eShotType) {
    case ST_COLUMN:
    case ST_SKULL:
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

    if (m_eShotType == ST_BOLT || m_eShotType == ST_SKULL || m_eShotType == ST_COLUMN) {
      m_vPosition += m_vSpeed * tpf;

      // check for collisions
      if (m_pMap->hitsTile(CTile::TF_UNPASSABLE, getWorldBoundingBox())) {
        // create
        if (m_eShotType == ST_BOLT) m_pMap->addExplosion(new CExplosion(m_pMap, getCenter(), CExplosion::ET_BOLT));
        else if (m_eShotType == ST_SKULL) m_pMap->addExplosion(new CExplosion(m_pMap, getCenter(), CExplosion::ET_SKULL));

        m_pMap->destroyShot(this);
      }
      if (m_pMap->outOfMap(getWorldBoundingBox())) {
        // just destroy it
        m_pMap->destroyShot(this);
      }

      hit();
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
  else if (m_pCatchedEnemy) {
    m_pCatchedEnemy->setStunned(true);
    m_pCatchedEnemy->addExternalForce((getCenter() - m_pCatchedEnemy->getCenter()) * 50);
    m_fTimer -= tpf;
    setAlpha(m_fTimer / COLUMN_CATCH_DURATION);
    if (m_fTimer <= 0) {
      m_pCatchedEnemy->setStunned(false);
      m_pCatchedEnemy = NULL;
      m_pMap->destroyShot(this);
    }
  }

  CAnimatedSprite::update(tpf);
}
void CShot::hit() {
  if (m_uiDamages & DMG_ENEMY) {
    for (auto *pEnemy : m_pMap->getEnemies()) {
      if (pEnemy->getWorldBoundingBox().collidesWith(getWorldBoundingBox())) {
        if (m_eShotType == ST_COLUMN) {
          m_pCatchedEnemy = pEnemy;
          m_pCatchedEnemy->setStunned(true);
          m_bLaunched = false;
          m_fTimer = COLUMN_CATCH_DURATION;
        }
        else {
          m_pMap->destroyShot(this);
        }

        pEnemy->takeDamage(SHOT_DAMAGE[m_eShotType]);

        return;
      }
    }
  }
  if (m_uiDamages & DMG_PLAYER) {
    if (m_pMap->getPlayer()->getWorldBoundingBox().collidesWith(getWorldBoundingBox())) {
      m_pMap->getPlayer()->takeDamage(SHOT_DAMAGE[m_eShotType]);
      m_pMap->destroyShot(this);
      return;
    }
  }
}
void CShot::writeToXMLElement(tinyxml2::XMLElement *pElement) {
  CAnimatedSprite::writeToXMLElement(pElement);
  pElement->SetAttribute("type", m_eShotType);
  pElement->SetAttribute("affected_by_gravity", m_bAffectedByGravity);
  SetAttribute(pElement, "speed", m_vSpeed);
  pElement->SetAttribute("direction", m_eShotDirection);
  pElement->SetAttribute("timer", m_fTimer);
  pElement->SetAttribute("launched", m_bLaunched);
  pElement->SetAttribute("damages", m_uiDamages);
  pElement->SetAttribute("catched_enemy", m_pCatchedEnemy->getID().c_str());
}
