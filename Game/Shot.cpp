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

#include "Shot.hpp"
#include "Constants.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Explosion.hpp"
#include "Enemy.hpp"
#include "Player.hpp"
#include "XMLHelper.hpp"
#include "IDGenerator.hpp"
#include "Message.hpp"

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
const Ogre::Real SHOT_DESTROY_ON_WALL_COLLISION[CShot::ST_COUNT] = {
  true,
  false,
  true,
  true,
};
const Ogre::Real BOMB_EXPLOSION_TIME = 5;
const Ogre::Real BOMB_EXPLOSION_RADIUS = 1.75;

CShot::CShot(CMap &map,
             const Ogre::Vector2 &vCenter,
             EShotTypes eShotType,
             EShotDirections eShotDirection,
             unsigned int uiDmg)
  :
  CAnimatedSprite(map,
		  CIDGenerator::nextID("Shot_"),
		  map.getShotsEntity(),
		  &map,
                  map.get2dManager(),
                  vCenter - SHOT_SIZE[eShotType] * 0.5,
                  SHOT_SIZE[eShotType]),
  m_eShotDirection(eShotDirection),
        //if (m_uiType == ST_BOLT) new CExplosion(m_Map, getCenter(), CExplosion::ET_BOLT);
        //else if (m_uiType == ST_SKULL) new CExplosion(m_Map, getCenter(), CExplosion::ET_SKULL);
  m_fTimer(0),
  m_eState(SS_NONE),
  m_uiDamages(uiDmg),
  m_pCatchedEnemy(NULL),
  m_sCatchedEnemyID("") {
  setType(eShotType);
  setGravity(SHOT_AFFECTED_BY_GRAVITY[m_uiType] ? c_fGravity : 0);
  constructor_impl();
}
CShot::CShot(CMap &map,
	     const tinyxml2::XMLElement *pElement)
  : CAnimatedSprite(map,
		    map.getShotsEntity(),
		    &map,
		    map.get2dManager(),
		    pElement,
		    SHOT_SIZE[EnumAttribute<EShotTypes>(pElement, "type", ST_COUNT, true)]),
    m_eShotDirection(EnumAttribute<EShotDirections>(pElement, "direction", SD_RIGHT, true)),
    m_fTimer(RealAttribute(pElement, "timer", 0)),
    m_eState(EnumAttribute<EShotStates>(pElement, "state", SS_NONE)),
    m_uiDamages(IntAttribute(pElement, "damages")),
    m_pCatchedEnemy(NULL),
    m_sCatchedEnemyID(Attribute(pElement, "catched_enemy", Ogre::StringUtil::BLANK)) {
  setGravity(SHOT_AFFECTED_BY_GRAVITY[m_uiType] ? c_fGravity : 0);
  constructor_impl();
}
void CShot::init() {
  CEntity::init();
  if (m_sCatchedEnemyID.size() > 0) {
    m_pCatchedEnemy = m_Map.getChildRecursive(m_sCatchedEnemyID);
  }
}
void CShot::constructor_impl() {
  CSpriteTexture::EMirrorTypes eMirrorType = CSpriteTexture::MIRROR_NONE;
  if (m_eShotDirection == SD_LEFT) {
    eMirrorType = CSpriteTexture::MIRROR_Y;
  }

  if (m_uiType == ST_BOLT) {
    CAnimatedSprite::init(1, 1);
    setupAnimation(SA_DEFAULT, "bolt", 2, eMirrorType, &getBoltTexture);
  }
  else if (m_uiType == ST_BOMB) {
    CAnimatedSprite::init(1, 2);
    setDefaultGetPath(&getBombTexture);
    setupAnimation(SA_DEFAULT, "bomb", -1, eMirrorType);
    setupAnimation(SA_LAUNCHED, "bomb_on", 6, eMirrorType);
  }
  else if (m_uiType == ST_SKULL) {
    CAnimatedSprite::init(1, 1);
    setupAnimation(SA_DEFAULT, "skull", 1, eMirrorType, &getSkullTexture);
  }
  else if (m_uiType == ST_COLUMN) {
    CAnimatedSprite::init(1, 1);
    setupAnimation(SA_DEFAULT, "column", 2, eMirrorType, &getColumnTexture);
    m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.4, 0));
    m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.2, 1.6));
  }

  changeCurrentAnimationSequence(SA_DEFAULT);
}
void CShot::launch(const Ogre::Vector2 &vInitialSpeed, unsigned int uiNewAnimationSequence) {
  m_fTimer = 0;
  m_eState = SS_LAUNCHED;
  setSpeed(vInitialSpeed * SHOT_SPEED[m_uiType]);
  setSleeping(false);
  if (uiNewAnimationSequence == SA_COUNT) {
    switch (m_uiType) {
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
  //setSleeping(m_eState == SS_LAUNCHED);
  if (m_eState == SS_LAUNCHED) {
    // change the position and speed
    //if (m_bAffectedByGravity) {
    //  m_vSpeed.y += c_fGravity * tpf;
    //}

    if (m_uiType == ST_BOLT || m_uiType == ST_SKULL || m_uiType == ST_COLUMN) {
      //m_vPosition += m_vSpeed * tpf;

      // check for collisions
      //if (m_Map.hitsTile(CTile::TF_UNPASSABLE, getWorldBoundingBox())) {
        // create
        //if (m_uiType == ST_BOLT) new CExplosion(m_Map, getCenter(), CExplosion::ET_BOLT);
        //else if (m_uiType == ST_SKULL) new CExplosion(m_Map, getCenter(), CExplosion::ET_SKULL);

        //destroy();
      //}
      //if (m_Map.outOfMap(getWorldBoundingBox())) {
        // just destroy it
        //destroy();
      //}

      hit();
    } else if (m_uiType == ST_BOMB) {
      m_fTimer += tpf;
      if (m_fTimer > BOMB_EXPLOSION_TIME) {
        m_Map.createExplosion(getCenter(), BOMB_EXPLOSION_RADIUS);
        destroy();
      } else {
	/*  bool bOnGround = false;
        CBoundingBox2d bbOverlap;
        m_vPosition.x += m_vSpeed.x * tpf;
        unsigned int uiCCD = m_Map.hitsTile(CTile::TF_UNPASSABLE,
                                              getWorldBoundingBox(),
                                              &bbOverlap);

        if (uiCCD & CCD_RIGHT && m_vSpeed.x > 0) {
          m_vPosition.x -= m_vSpeed.x * tpf;
          m_vSpeed.x *= -0.5;
        } else if (uiCCD & CCD_LEFT && m_vSpeed.x < 0) {
          m_vPosition.x -= m_vSpeed.x * tpf;
          m_vSpeed.x *= -0.5;
        }

        m_vPosition.y += m_vSpeed.y * tpf;
        uiCCD = m_Map.hitsTile(CTile::TF_UNPASSABLE,
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
          m_vSpeed.x -= m_vSpeed.x * 10 * tpf;
	  }*/
      }
    }
  }
  else if (m_eState == SS_ENEMY_CAUGHT) {
    if (m_pCatchedEnemy) {
      dynamic_cast<CEnemy*>(m_pCatchedEnemy)->setStunned(true);
      dynamic_cast<CEnemy*>(m_pCatchedEnemy)->addExternalForce((getCenter() - m_pCatchedEnemy->getCenter()) * 50);
    }
    m_fTimer -= tpf;
    setAlpha(m_fTimer / COLUMN_CATCH_DURATION);
    if (m_fTimer <= 0) {
      if (m_pCatchedEnemy) {
        dynamic_cast<CEnemy*>(m_pCatchedEnemy)->setStunned(false);
	m_pCatchedEnemy = NULL;
      }
      destroy();
    }
  }

  CAnimatedSprite::update(tpf);
}
void CShot::handleMessage(const CMessage &message) {
  switch (message.getType()) {
  case CMessage::MT_ENTITY_DESTROYED:
    if (m_pCatchedEnemy == message.getEntity()) {
      m_pCatchedEnemy = NULL;
    }
    break;
  default:
    break;
  }
}
void CShot::hit() {
  if (m_uiDamages & DMG_ENEMY) {
    for (auto *pEnemy : m_Map.getEnemies()) {
      if (pEnemy->getWorldBoundingBox().collidesWith(getWorldBoundingBox())) {
        if (m_uiType == ST_COLUMN) {
          m_pCatchedEnemy = pEnemy;
          dynamic_cast<CEnemy*>(m_pCatchedEnemy)->setStunned(true);
          m_eState = SS_ENEMY_CAUGHT;
          m_fTimer = COLUMN_CATCH_DURATION;
        }
        else {
          destroy();
        }

        dynamic_cast<CHitableObject*>(pEnemy)->takeDamage(SHOT_DAMAGE[m_uiType]);

        return;
      }
    }
  }
  if (m_uiDamages & DMG_PLAYER) {
    if (m_Map.getPlayer()->getWorldBoundingBox().collidesWith(getWorldBoundingBox())) {
      m_Map.getPlayer()->takeDamage(SHOT_DAMAGE[m_uiType]);
      destroy();
      return;
    }
  }
}

void CShot::writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const {
  CAnimatedSprite::writeToXMLElement(pElement, eStyle);
  pElement->SetAttribute("type", m_uiType);
  pElement->SetAttribute("direction", m_eShotDirection);
  pElement->SetAttribute("damages", m_uiDamages);

  if (eStyle == OS_FULL) {
    pElement->SetAttribute("timer", m_fTimer);
    pElement->SetAttribute("state", m_eState);
    if (m_pCatchedEnemy) {
      pElement->SetAttribute("catched_enemy", m_pCatchedEnemy->getID().c_str());
    }
  }
}

void CShot::handleOutOfMap() {
  destroy();
}


bool CShot::handleHorizontalTileCollision(unsigned int uiCCD, Ogre::Real tpf) {
  if (SHOT_DESTROY_ON_WALL_COLLISION[m_uiType]) {
    createExplosion();
    destroy();
    return false;
  }
  else {
    return CPhysicsEntity::handleHorizontalTileCollision(uiCCD, tpf);
  }
}

bool CShot::handleVerticalTileCollision(unsigned int uiCCD, Ogre::Real tpf) {
  if (SHOT_DESTROY_ON_WALL_COLLISION[m_uiType]) {
    createExplosion();
    destroy();
    return false;
  }
  else {
    return CPhysicsEntity::handleVerticalTileCollision(uiCCD, tpf);
  }
}

void CShot::createExplosion() {
  if (m_uiType == ST_BOLT)
    new CExplosion(m_Map, getCenter(), CExplosion::ET_BOLT);
  else if (m_uiType == ST_SKULL)
    new CExplosion(m_Map, getCenter(), CExplosion::ET_SKULL);
}
