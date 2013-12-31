#include "Enemy.hpp"
#include "Map.hpp"
#include "Util.hpp"
#include "Tile.hpp"
#include "DebugDrawer.hpp"
#include "Explosion.hpp"
#include "Player.hpp"
#include "Constants.hpp"

const Ogre::Vector2 ENEMY_SIZE[CEnemy::ET_COUNT] = {
  Ogre::Vector2(1, 1),
  Ogre::Vector2(1, 2),
  Ogre::Vector2(4, 2)
};
const Ogre::Real ENEMY_SPEED[CEnemy::ET_COUNT] = {
  6.0,
  2.0,
  6.0
};
const Ogre::Real ENEMY_DAMAGE[CEnemy::ET_COUNT] = {
  0.1,
  0.2,
  0.3
};

CEnemy::CEnemy(CMap &map,
	       const Ogre::Vector2 &vPosition,
	       EEnemyTypes eEnemyType,
	       Ogre::Real fDirection,
	       Ogre::Real fHitpoints)
  : CAnimatedSprite(&map, map.get2dManager(), vPosition, ENEMY_SIZE[eEnemyType]),
    CHitableObject(fHitpoints),
    m_HPBar(&map, map.get2dManager()),
    m_Map(map),
    m_eEnemyType(eEnemyType),
    m_vSpeed(Ogre::Vector2(fDirection * ENEMY_SPEED[eEnemyType], 0)),
    m_bOnGround(false) {
  setup();
  m_HPBar.setSize(Ogre::Vector2(m_vSize.x, m_HPBar.getSize().y));
}
void CEnemy::update(Ogre::Real tpf) {
  CAnimatedSprite::update(tpf);

  unsigned int ccd = getWorldBoundingBox().collidesWith(m_Map.getPlayer()->getWorldBoundingBox());
  if (m_bOnGround && ccd != CCD_NONE) {
    // Player collision, attack him!
    changeCurrentAnimationSequence((ccd & CCD_LEFT) ? AS_ATTACK_LEFT : AS_ATTACK_RIGHT);
  }
  else {
    updateKI();

    Ogre::Real fPenetration = 0;

    m_vSpeed.y += c_fGravity * tpf;
    m_vPosition.y += m_vSpeed.y * tpf;

    m_bOnGround = false;
    if (m_vSpeed.y < 0) {
        fPenetration = m_Map.hitsTile(CCD_BOTTOM, CTile::TF_UNPASSABLE, getWorldBoundingBox());
        if (fPenetration != 0) {
          m_bOnGround = true;
        }
    }

    if (fPenetration != 0) {
        m_vPosition.y -= fPenetration;
        m_vSpeed.y = 0;
    }
    // move enemy
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
  }

#ifdef DEBUG_CHARACTER_BOUNDING_BOXES
  CDebugDrawer::getSingleton().draw(getWorldBoundingBox());
#endif

  if (getMaximumHitpoints() > getHitpoints()) {
    m_HPBar.setCenter(getCenter() + Ogre::Vector2(0, getSize().y / 2));
    m_HPBar.update(tpf);
  }
}
void CEnemy::updateKI() {
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

      if (bFlipWalkDirection) {
        m_vSpeed.x *= -1;
      }
    }
  }
}
void CEnemy::setup() {
  init(ENEMY_SPEED[m_eEnemyType], AS_COUNT);
  switch (m_eEnemyType) {
  case ET_GREEN_MONSTER:
    m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.1, 0));
    m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.8, 0.9));
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
    setupAnimation(AS_JUMP_LEFT, "jump_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "jump_right", 1);
    break;
  case ET_KNIGHT:
    setDefaultGetPath(&getEnemyTexturePath<2>);
    setupAnimation(AS_WALK_LEFT, "walk_right", {0, 1, 2, 3, 0, 4, 5, 6}, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", {0, 1, 2, 3, 0, 4, 5, 6});
    setupAnimation(AS_ATTACK_LEFT, "attack_right", {0, 1, 2, 1}, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_ATTACK_RIGHT, "attack_right", {0, 1, 2, 1});
    setupAnimation(AS_JUMP_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "walk_right", 1);
    break;
  case ET_BEAR:
    setDefaultGetPath(&getEnemyTexturePath<3>);
    setupAnimation(AS_WALK_LEFT, "walk_right", 6, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", 6);
    setupAnimation(AS_ATTACK_LEFT, "attack_right", {0, 1, 2, 1}, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_ATTACK_RIGHT, "attack_right", {0, 1, 2, 1});
    setupAnimation(AS_JUMP_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_JUMP_RIGHT, "walk_right", 1);
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Enemy type '" + Ogre::StringConverter::toString(m_eEnemyType) + "' is unknown", __FILE__);
  }
}
void CEnemy::damageTakenCallback() {
  m_HPBar.show();
  m_HPBar.setValue(getHitpoints() / getMaximumHitpoints());
}

void CEnemy::killedByDamageCallback() {
  m_Map.destroyEnemy(this);
  m_Map.addExplosion(new CExplosion(&m_Map, getCenter(), static_cast<CExplosion::EExplosionTypes>(m_eEnemyType + CExplosion::ET_GREEN_MONSTER)));
}

void CEnemy::animationTextureChangedCallback(unsigned int uiOldText, unsigned uiNewText) {
  if (uiOldText == m_AnimationSequences[m_uiCurrentAnimationSequence].size() - 1 && uiNewText == 0) {
    if (m_uiCurrentAnimationSequence == AS_ATTACK_RIGHT || m_uiCurrentAnimationSequence == AS_ATTACK_LEFT) {
      m_Map.getPlayer()->takeDamage(ENEMY_DAMAGE[m_eEnemyType]);
    }
  }
}
