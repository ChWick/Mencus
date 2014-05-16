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

#include "Enemy.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Tile.hpp"
#include "DebugDrawer.hpp"
#include "Explosion.hpp"
#include "Player.hpp"
#include "Constants.hpp"
#include "Shot.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

Ogre::Real ENEMY_MAX_RANGED_ATTACK_DISTANCE = 5;

const Ogre::Vector2 ENEMY_SIZE[CEnemy::ET_COUNT] = {
  Ogre::Vector2(1, 1),
  Ogre::Vector2(1, 2),
  Ogre::Vector2(4, 2),
  Ogre::Vector2(1, 1),
  Ogre::Vector2(1, 2),
  Ogre::Vector2(1, 1)
};
const Ogre::Real ENEMY_SPEED[CEnemy::ET_COUNT] = {
  6.0,
  2.0,
  6.0,
  2.0,
  2.0,
  2.0
};
const Ogre::Real ENEMY_DAMAGE[CEnemy::ET_COUNT] = {
  0.1,
  0.2,
  0.3,
  0.4,
  0.5,
  1.0
};
CEnemy::EAttackType ENEMY_ATTACK_TYPES[CEnemy::ET_COUNT] = {
  CEnemy::AT_MELEE,
  CEnemy::AT_MELEE,
  CEnemy::AT_MELEE,
  CEnemy::AT_MELEE,
  CEnemy::AT_RANGED,
  CEnemy::AT_ALL,
};

CEnemy::CEnemy(CMap &map,
	       CEntity *pParent,
	       const Ogre::Vector2 &vPosition,
	       EEnemyTypes eEnemyType,
	       Ogre::Real fDirection,
	       Ogre::Real fHitpoints,
	       bool bJumps)
  : CAnimatedSprite(map,
		    CIDGenerator::nextID("Enemy_"),
		    pParent,
		    &map,
		    map.get2dManager(),
		    vPosition,
		    ENEMY_SIZE[eEnemyType]),
    CHitableObject(fHitpoints),
    m_vSpeed(Ogre::Vector2(fDirection * ENEMY_SPEED[eEnemyType], 0)),
    m_bOnGround(false),
  m_HPBar(map, this, &map, map.get2dManager()),
    m_bJumps(bJumps),
    m_vExternalForce(Ogre::Vector2::ZERO),
    m_bStunned(false),
    m_bAtLeastOneDamageDone(true) {
  setType(eEnemyType);
  setup();
  m_HPBar.setSize(Ogre::Vector2(m_vSize.x, m_HPBar.getSize().y));
}
CEnemy::CEnemy(CMap &map,
	       CEntity *pParent,
	       const tinyxml2::XMLElement *pElem)
  : CAnimatedSprite(map,
		    pParent,
		    &map,
		    map.get2dManager(),
		    pElem,
		    ENEMY_SIZE[EnumAttribute(pElem, "type", ET_COUNT)]),
    CHitableObject(pElem),
    m_vSpeed(Vector2Attribute(pElem,
			      "speed",
			      Ogre::Vector2::UNIT_X * RealAttribute(pElem, "direction", 1)
			      * ENEMY_SPEED[EnumAttribute(pElem, "type", ET_COUNT)])),
    m_bOnGround(false),
    m_HPBar(map, this, &map, map.get2dManager()),
    m_bJumps(BoolAttribute(pElem, "jumps", true)),
    m_vExternalForce(Ogre::Vector2::ZERO),
    m_bStunned(BoolAttribute(pElem, "stunned", false)),
    m_bAtLeastOneDamageDone(BoolAttribute(pElem, "oneDmgDone", true)) {

  setup();
  m_HPBar.setSize(Ogre::Vector2(m_vSize.x, m_HPBar.getSize().y));
}
void CEnemy::update(Ogre::Real tpf) {
  CAnimatedSprite::update(tpf);

  bool bWalk = readyForWalking();
  if (m_bOnGround) {
    if (ENEMY_ATTACK_TYPES[m_uiType] & AT_MELEE) {
      unsigned int ccd = getWorldBoundingBox().collidesWith(m_Map.getPlayer()->getWorldBoundingBox());
      if (ccd != CCD_NONE) {
        // Player collision, attack him!
        changeCurrentAnimationSequence((getCenter().x > m_Map.getPlayer()->getCenter().x) ? AS_ATTACK_LEFT : AS_ATTACK_RIGHT);
        bWalk = false;
        m_bAtLeastOneDamageDone = false;
      }
    }
    else if (ENEMY_ATTACK_TYPES[m_uiType] & AT_RANGED) {
      ECollisionCheckDirections eCCD = m_Map.playerVisibleFromPoint(getCenter(), ENEMY_MAX_RANGED_ATTACK_DISTANCE, CCD_HORIZONTAL);
      if (eCCD & CCD_LEFT) {
        changeCurrentAnimationSequence(AS_RANGED_ATTACK_LEFT);
        bWalk = false;
        m_bAtLeastOneDamageDone = false;
      }
      else if (eCCD & CCD_RIGHT) {
        changeCurrentAnimationSequence(AS_RANGED_ATTACK_RIGHT);
        bWalk = false;
        m_bAtLeastOneDamageDone = false;
      }
    }
  }
  if (bWalk) {
    if (!m_bStunned) {
      updateKI(tpf);
    }

    Ogre::Real fPenetration = 0;

    m_vSpeed.y += (m_vExternalForce.y + c_fGravity) * tpf;
    m_vPosition.y += m_vSpeed.y * tpf;

    if (m_vSpeed.y != 0) {
      m_bOnGround = false;
      if (m_vSpeed.y < 0) {
        fPenetration = m_Map.hitsTile(CCD_BOTTOM, CTile::TF_UNPASSABLE, getWorldBoundingBox());
        if (fPenetration != 0) {
          m_bOnGround = true;
        }
      }
    }

    if (fPenetration != 0) {
        m_vPosition.y -= fPenetration;
        m_vSpeed.y = 0;
    }
    // move enemy
    m_vSpeed.x += m_vExternalForce.x * tpf;
    m_vPosition.x += m_vSpeed.x * tpf;

    if (m_vSpeed.x < 0) {
      fPenetration = m_Map.hitsTile(CCD_LEFT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
    }
    else {
      fPenetration = m_Map.hitsTile(CCD_RIGHT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
    }

    if (m_Map.collidesWithMapMargin(getWorldBoundingBox())) {
      fPenetration = m_vSpeed.x * tpf;
    }

    if (fPenetration != 0) {
      m_vPosition.x -= fPenetration;
      m_vSpeed.x *= -1;
    }

    if (m_bOnGround) {
      if (m_vSpeed.x < 0) {
        changeCurrentAnimationSequence(AS_WALK_LEFT);
      }
      else {
        changeCurrentAnimationSequence(AS_WALK_RIGHT);
      }
    }
    else {
      if (m_vSpeed.x < 0) changeCurrentAnimationSequence(AS_JUMP_LEFT);
      else changeCurrentAnimationSequence(AS_JUMP_RIGHT);
    }

    m_vSpeed.x -= m_vSpeed.x * tpf * 5;
  }

#ifdef DEBUG_CHARACTER_BOUNDING_BOXES
  CDebugDrawer::getSingleton().draw(getWorldBoundingBox());
#endif

  if (getMaximumHitpoints() > getHitpoints()) {
    m_HPBar.setCenter(getCenter() + Ogre::Vector2(0, getSize().y / 2));
    m_HPBar.update(tpf);
  }
  m_vExternalForce = Ogre::Vector2::ZERO;
}
void CEnemy::render(Ogre::Real tpf) {
  if (getMaximumHitpoints() > getHitpoints()) {
      m_HPBar.render(tpf);
  }
  CAnimatedSprite::render(tpf);
}
void CEnemy::updateKI(Ogre::Real tpf) {
  if (m_bOnGround) {
    CBoundingBox2d bb(getWorldBoundingBox());
    bb = bb.translate(Ogre::Vector2(0, -0.1));
    int x = static_cast<int>(bb.getCenter().x);
    if (m_vSpeed.x > 0) {
      //x = static_cast<int>(bb.getPosition().x + bb.getSize().x);
    }
    int y = static_cast<int>(bb.getPosition().y);
    CTile *pTile(m_Map.getTile(x, y));
    if (pTile->getTileFlags() & CTile::TF_PASSABLE) {
      bool bFlipWalkDirection = true;
      if (m_bJumps) {
        // search the next unpassable tile and apply jump speed in this case
        for (int iDeltaJumpWidth = 1; iDeltaJumpWidth <= 3; iDeltaJumpWidth++) {
          if (m_vSpeed.x < 0) {
            x = static_cast<int>(bb.getPosition().x) - iDeltaJumpWidth;
          }
          else {
            x = static_cast<int>(bb.getPosition().x + bb.getSize().x) + iDeltaJumpWidth;
          }
          if (!m_Map.isInMap(x, y)) {break;}
          pTile = m_Map.getTile(x, y);
#ifdef DEBUG_KI
          CDebugDrawer::getSingleton().draw(pTile, CDebugDrawer::DT_BLUE);
#endif
          if (pTile->getTileFlags() & CTile::TF_UNPASSABLE) {
            bFlipWalkDirection = false;
            Ogre::Real t = (iDeltaJumpWidth + 0.2) / m_vSpeed.x;
            m_vSpeed.y = abs(0.5 * c_fGravity * t);
            break;
          }
        }
      }

      if (bFlipWalkDirection) {
        m_vSpeed.x *= -1;
	// make the movement step backwards
	m_vPosition.x += m_vSpeed.x * tpf;
      }
    }
  }
  if (m_vSpeed.x != 0) {
    m_vSpeed.x = ENEMY_SPEED[m_uiType] * m_vSpeed.x / abs(m_vSpeed.x);
  }
}
void CEnemy::setup() {
  CAnimatedSprite::init(ENEMY_SPEED[m_uiType], AS_COUNT);
  CHitableObject::init();
  switch (m_uiType) {
  case ET_GREEN_MONSTER:
    m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.1, 0));
    m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.8, 0.9));
    setDefaultGetPath(&getEnemyTexturePath<ET_GREEN_MONSTER>);
    setupAnimation(AS_WALK_LEFT, "walk_right", 4, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", 4);
    {
      CSpriteTexture &attackLeft
	= addTextureToAnimationSequence(AS_ATTACK_LEFT,
					getEnemyTexturePath<ET_GREEN_MONSTER>("attack_right", 1));
      attackLeft.mirror(CSpriteTexture::MIRROR_Y);
      attackLeft.setSpriteOffset(Ogre::Vector2(-1, 0));
      attackLeft.setSpriteScale(Ogre::Vector2(2, 1));
      addTextureToAnimationSequence(AS_ATTACK_LEFT,
				    getEnemyTexturePath<ET_GREEN_MONSTER>("walk_right", 1)).mirror(CSpriteTexture::MIRROR_Y);
    }
    {
      CSpriteTexture &attackRight
	= addTextureToAnimationSequence(AS_ATTACK_RIGHT,
					       getEnemyTexturePath<ET_GREEN_MONSTER>("attack_right", 1));
      attackRight.setSpriteScale(Ogre::Vector2(2, 1));
      addTextureToAnimationSequence(AS_ATTACK_RIGHT, getEnemyTexturePath<ET_GREEN_MONSTER>("walk_right", 1));
    }
    setupAnimation(AS_JUMP_LEFT, "jump_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "jump_right", 1);
    break;
  case ET_KNIGHT:
    m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.2, 0));
    m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.6, 1.9));
    setDefaultGetPath(&getEnemyTexturePath<ET_KNIGHT>);
    setupAnimation(AS_WALK_LEFT, "walk_right", {0, 1, 2, 3, 0, 4, 5, 6}, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", {0, 1, 2, 3, 0, 4, 5, 6});
    for (int i = 0; i < 8; i++) {
      getTexture(AS_WALK_LEFT, i).setSpriteOffset(Ogre::Vector2(-0.2, 0));
      getTexture(AS_WALK_RIGHT, i).setSpriteOffset(Ogre::Vector2(0.2, 0));
    }
    setupAnimation(AS_ATTACK_LEFT, "attack_right", {0, 1, 2, 1}, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_ATTACK_RIGHT, "attack_right", {0, 1, 2, 1});
    for (int i = 0; i < 4; i++) {
      getTexture(AS_ATTACK_LEFT, i).setSpriteScale(Ogre::Vector2(2, 1));
      getTexture(AS_ATTACK_LEFT, i).setSpriteOffset(Ogre::Vector2(-1.2, 0));
      getTexture(AS_ATTACK_RIGHT, i).setSpriteScale(Ogre::Vector2(2, 1));
      getTexture(AS_ATTACK_RIGHT, i).setSpriteOffset(Ogre::Vector2(0.2, 0));
    }
    setupAnimation(AS_JUMP_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "walk_right", 1);
    for (int i = 0; i < 1; i++) {
      getTexture(AS_JUMP_LEFT, i).setSpriteOffset(Ogre::Vector2(-0.2, 0));
      getTexture(AS_JUMP_RIGHT, i).setSpriteOffset(Ogre::Vector2(0.2, 0));
    }
    break;
  case ET_BEAR:
    m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.4, 0));
    m_bbRelativeBoundingBox.setSize(Ogre::Vector2(3.2, 1.7));
    setDefaultGetPath(&getEnemyTexturePath<ET_BEAR>);
    setupAnimation(AS_WALK_LEFT, "walk_right", 6, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", 6);
    setupAnimation(AS_ATTACK_LEFT, "attack_right", {0, 1, 2, 1}, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_ATTACK_RIGHT, "attack_right", {0, 1, 2, 1});
    setupAnimation(AS_JUMP_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "walk_right", 1);
    break;
  case ET_GHOST:
    setDefaultGetPath(&getEnemyTexturePath<ET_GHOST>);
    setupAnimation(AS_WALK_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", 1);
    setupAnimation(AS_ATTACK_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_ATTACK_RIGHT, "walk_right", 1);
    setupAnimation(AS_JUMP_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "walk_right", 1);
    break;
  case ET_DARK_MAGICAN:
    setDefaultGetPath(&getEnemyTexturePath<ET_DARK_MAGICAN>);
    setupAnimation(AS_WALK_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", 1);
    setupAnimation(AS_RANGED_ATTACK_LEFT, "attack_right", 2, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_RANGED_ATTACK_RIGHT, "attack_right", 2);
    setupAnimation(AS_JUMP_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "walk_right", 1);
    break;
  case ET_SANTA:
    setDefaultGetPath(&getEnemyTexturePath<ET_SANTA + 1>);
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Enemy type '" + Ogre::StringConverter::toString(m_uiType) + "' is unknown", __FILE__);
  }
}
void CEnemy::damageTakenCallback(Ogre::Real fHitpoints) {
  m_HPBar.show();
  m_HPBar.setValue(getHitpoints() / getMaximumHitpoints());
}

void CEnemy::killedByDamageCallback() {
  this->destroy();
  new CExplosion(m_Map, getCenter(), static_cast<CExplosion::EExplosionTypes>(m_uiType + CExplosion::ET_GREEN_MONSTER));
}

void CEnemy::animationTextureChangedCallback(unsigned int uiOldText, unsigned uiNewText) {
  if (uiOldText == m_AnimationSequences[m_uiCurrentAnimationSequence].size() - 1 && uiNewText == 0) {
    if (m_uiCurrentAnimationSequence == AS_ATTACK_RIGHT || m_uiCurrentAnimationSequence == AS_ATTACK_LEFT) {
      m_Map.getPlayer()->takeDamage(ENEMY_DAMAGE[m_uiType]);
    }
    else if (m_uiCurrentAnimationSequence == AS_RANGED_ATTACK_LEFT) {
      CShot *pShot = new CShot(m_Map,
			       getCenter(),
			       CShot::ST_SKULL,
			       CShot::SD_LEFT,
			       CShot::DMG_PLAYER);
      pShot->launch(Ogre::Vector2(-1, 0));
    }
    else if (m_uiCurrentAnimationSequence == AS_RANGED_ATTACK_RIGHT) {
      CShot *pShot = new CShot(m_Map,
			       getCenter(),
			       CShot::ST_SKULL,
			       CShot::SD_RIGHT,
			       CShot::DMG_PLAYER);
      pShot->launch(Ogre::Vector2(+1, 0));
    }
    m_bAtLeastOneDamageDone = true;
  }
}

bool CEnemy::readyForWalking() {
  if (m_uiCurrentAnimationSequence == AS_ATTACK_RIGHT || m_uiCurrentAnimationSequence == AS_ATTACK_LEFT ||
      m_uiCurrentAnimationSequence == AS_RANGED_ATTACK_LEFT || m_uiCurrentAnimationSequence == AS_RANGED_ATTACK_RIGHT) {
    return m_uiCurrentAnimationTexture == 0 && m_bAtLeastOneDamageDone;
  }
  return true;
}

void CEnemy::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  pElem->SetAttribute("direction", getDirection());
  pElem->SetAttribute("jumps", mayJump());

  if (eStyle == OS_FULL) {
    SetAttribute(pElem, "speed", getSpeed());
  }

  CHitableObject::writeToXMLElement(pElem, eStyle);
  CAnimatedSprite::writeToXMLElement(pElem, eStyle);
}
Ogre::String CEnemy::getPreviewImageName(int iEnemyType) {
  switch (iEnemyType) {
  case ET_GREEN_MONSTER:
    return getEnemyTexturePath<ET_GREEN_MONSTER>("walk_right_1");
    break;
  case ET_KNIGHT:
    return getEnemyTexturePath<ET_KNIGHT>("walk_right_1");
    break;
  case ET_BEAR:
    return getEnemyTexturePath<ET_BEAR>("walk_right_1");
    break;
  case ET_GHOST:
    return getEnemyTexturePath<ET_GHOST - 1>("walk_right_2"); // debugging
    break;
  case ET_DARK_MAGICAN:
    return getEnemyTexturePath<ET_DARK_MAGICAN>("walk_right_1");
    break;
  case ET_SANTA:
    return getEnemyTexturePath<ET_SANTA - 1>("attack_right_1"); // debugging
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Enemy type '" + Ogre::StringConverter::toString(iEnemyType) + "' is unknown", __FILE__);
  }
  return Ogre::StringUtil::BLANK;
}
