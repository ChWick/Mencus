#include "Player.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Constants.hpp"
#include "DebugDrawer.hpp"
#include "Shot.hpp"

const Ogre::Vector2 PLAYER_BOLT_OFFSET_RIGHT(0.2, 0.5);
const Ogre::Vector2 PLAYER_BOLT_OFFSET_LEFT(PLAYER_BOLT_OFFSET_RIGHT * Ogre::Vector2(-1, 1));

const Ogre::Vector2 PLAYER_BOMB_OFFSET(0, 0.9);
const Ogre::Vector2 PLAYER_BOMB_THROW_DIRECTION_RIGHT(0.9, 0.1);
const Ogre::Vector2 PLAYER_BOMB_THROW_DIRECTION_LEFT(PLAYER_BOMB_THROW_DIRECTION_RIGHT * Ogre::Vector2(-1, 1));
const Ogre::Real PLAYER_BOMB_DEFAULT_THROW_STRENGTH(1);
const Ogre::Real PLAYER_BOMB_THROW_STRENGTH_INCREASE(3);
const Ogre::Real PLAYER_BOMB_MAX_TRHOW_STRENGTH(5);

CPlayer::CPlayer(CMap *pMap, Ogre2dManager *pSpriteManager) 
 :
  CAnimatedSprite(pMap, pSpriteManager, Ogre::Vector2(0, 0), Ogre::Vector2(1, 2)),
  m_pMap(pMap),
  m_bLeftPressed(false),
  m_bRightPressed(false),
  m_bAttackPressed(false),
  m_bJumpPressed(false),
  m_fMaxWalkSpeed(4),
  m_fInitialJumpSpeed(10),
  m_vCurrentSpeed(0, 0),
  m_bOnGround(true),
  m_bJumping(false),
  m_eLastDirection(LD_RIGHT),
  m_uiCurrentWeapon(W_BOMB),
  m_pBomb(NULL) {
  CInputListenerManager::getSingleton().addInputListener(this);
  init(1, ANIM_COUNT);
  setupAnimations();
  m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.2, 0));
  m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.6, 1.8));
}
CPlayer::~CPlayer() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CPlayer::setupAnimations() {
  setDefaultGetPath(&getPlayerTexturePath);
  setupAnimation(ANIM_WALK_RIGHT, "walk_right", 4);
  setupAnimation(ANIM_WALK_LEFT, "walk_right", 4, CSpriteTexture::MIRROR_Y);
  setupAnimation(ANIM_STAND_RIGHT, "walk_right", 1);
  setupAnimation(ANIM_STAND_LEFT, "walk_right", 1, CSpriteTexture::MIRROR_Y);
  setupAnimation(ANIM_ATTACK_RIGHT, "attack_right", {0, 1, 2, 3, 3, 3, 3, 2, 1, 0});
  setupAnimation(ANIM_ATTACK_LEFT, "attack_right", {0, 1, 2, 3, 3, 3, 3, 2, 1, 0}, CSpriteTexture::MIRROR_Y);
  setupAnimation(ANIM_JUMP_RIGHT, "jump_right", 1);
  setupAnimation(ANIM_JUMP_LEFT, "jump_right", 1, CSpriteTexture::MIRROR_Y);
  setupAnimation(ANIM_FALL_RIGHT, "fall_right", 1);
  setupAnimation(ANIM_FALL_LEFT, "fall_right", 1, CSpriteTexture::MIRROR_Y);

  setupAnimation(ANIM_HOLD_LEFT, "throw_right", 1, CSpriteTexture::MIRROR_Y);
  setupAnimation(ANIM_HOLD_RIGHT, "throw_right", 1);
  setupAnimation(ANIM_THROW_RIGHT, "throw_right", 2);
  setupAnimation(ANIM_THROW_LEFT, "throw_right",2, CSpriteTexture::MIRROR_Y);

  // Fixes for attack sprite texutres
  for (int i = 2; i <= 7; i++) {
    getTexture(ANIM_ATTACK_LEFT, i).setSpriteScale(Ogre::Vector2(2, 1));
    getTexture(ANIM_ATTACK_LEFT, i).setSpriteOffset(Ogre::Vector2(-1, 0));
    getTexture(ANIM_ATTACK_RIGHT, i).setSpriteScale(Ogre::Vector2(2, 1));
  }
}
void CPlayer::update(Ogre::Real tpf) {
  if (m_bLeftPressed) {
    m_vCurrentSpeed.x = -m_fMaxWalkSpeed;
  }
  else if (m_bRightPressed) {
    m_vCurrentSpeed.x = m_fMaxWalkSpeed;
  }
  else {
    m_vCurrentSpeed.x = 0;
  }

  m_vCurrentSpeed.y += c_fGravity * tpf;
  
  if (m_vCurrentSpeed.y > 0 && !m_bJumpPressed) {
    // If the user wants to cancel the jump, do a higher acceleration
    m_vCurrentSpeed.y += c_fGravity * tpf; // twice gravity now
  }

  if (!m_pBomb) {
    Ogre::Real fPenetration = 0;
    // move and check here only y direction movement
    m_vPosition.y += m_vCurrentSpeed.y * tpf;

    if (m_vCurrentSpeed.y < 0) {
      m_bOnGround = false;
      m_bJumping = true;
      fPenetration = m_pMap->hitsTile(CCD_BOTTOM, CTile::TF_UNPASSABLE, getWorldBoundingBox());
      if (fPenetration != 0) {
	m_bOnGround = true;
	m_bJumping = false;
      }
    }
    else if (m_vCurrentSpeed.y > 0) {
      fPenetration = m_pMap->hitsTile(CCD_TOP, CTile::TF_UNPASSABLE, getWorldBoundingBox());
    }

    if (fPenetration != 0) {
      m_vCurrentSpeed.y = 0;
      m_vPosition.y -= fPenetration;
    }


    // move and check here only x direction movement
    fPenetration = 0;
    m_vPosition.x += m_vCurrentSpeed.x * tpf;
  
    if (m_vCurrentSpeed.x < 0) {
      fPenetration = m_pMap->hitsTile(CCD_LEFT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
      m_eLastDirection = LD_LEFT;
    }
    else if (m_vCurrentSpeed.x > 0) {
      fPenetration += m_pMap->hitsTile(CCD_RIGHT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
      m_eLastDirection = LD_RIGHT;
    }

    if (fPenetration != 0) {
      m_vPosition.x -= fPenetration;
      m_vCurrentSpeed.x = 0;
    }
  }

  if (m_uiCurrentAnimationSequence == ANIM_HOLD_LEFT || m_uiCurrentAnimationSequence == ANIM_HOLD_RIGHT) {
    if (!m_bAttackPressed) {
      // release bomb
      m_pBomb->launch(((m_eLastDirection == LD_LEFT) ? PLAYER_BOMB_THROW_DIRECTION_LEFT : PLAYER_BOMB_THROW_DIRECTION_RIGHT) * m_fBombThrowStrength);
      m_pBomb = NULL;
      if (m_eLastDirection == LD_LEFT) {
	changeCurrentAnimationSequence(ANIM_THROW_LEFT);
      }
      else {
	changeCurrentAnimationSequence(ANIM_THROW_RIGHT);
      }
    }
    else {
      // increase throw strength
      m_fBombThrowStrength += PLAYER_BOMB_THROW_STRENGTH_INCREASE * tpf;
      m_fBombThrowStrength = min(m_fBombThrowStrength, PLAYER_BOMB_MAX_TRHOW_STRENGTH);
    }
  }
  else if (m_uiCurrentAnimationSequence == ANIM_THROW_RIGHT || m_uiCurrentAnimationSequence == ANIM_THROW_LEFT){
    // do nothing, wait for finished
  }
  else if (m_bOnGround) {
    if (m_bAttackPressed && m_vCurrentSpeed.x == 0) {
      if (m_uiCurrentWeapon == W_BOLT) {
	if (m_eLastDirection == LD_LEFT) {
	  changeCurrentAnimationSequence(ANIM_ATTACK_LEFT);
	}
	else {
	  changeCurrentAnimationSequence(ANIM_ATTACK_RIGHT);
	}
      }
      else if (m_uiCurrentWeapon == W_BOMB) {
	if (!m_pBomb) {
	  m_pBomb = new CShot(m_pMap, m_pSpriteManager, getCenter(), CShot::ST_BOMB, (m_eLastDirection == LD_LEFT) ? CShot::SD_LEFT : CShot::SD_RIGHT);
	  m_pMap->addShot(m_pBomb);
	}
	m_pBomb->setCenter(getCenter() + PLAYER_BOMB_OFFSET);
	if (m_eLastDirection == LD_LEFT) {
	  changeCurrentAnimationSequence(ANIM_HOLD_LEFT);
	}
	else {
	  changeCurrentAnimationSequence(ANIM_HOLD_RIGHT);
	}
      }
    }
    else {
      if (m_vCurrentSpeed.x < 0) {
	changeCurrentAnimationSequence(ANIM_WALK_LEFT);
      }
      else if (m_vCurrentSpeed.x > 0) {
	changeCurrentAnimationSequence(ANIM_WALK_RIGHT);
      }
      else {
	if (m_eLastDirection == LD_LEFT) {
	  changeCurrentAnimationSequence(ANIM_STAND_LEFT);
	}
	else {
	  changeCurrentAnimationSequence(ANIM_STAND_RIGHT);
	}
      }
    }
  }
  else if (m_bJumping) {
    if (m_vCurrentSpeed.y > -5.0) { // fall animation appears at a certain fall speed
      if (m_eLastDirection == LD_LEFT) {
	changeCurrentAnimationSequence(ANIM_JUMP_LEFT);
      }
      else {
	changeCurrentAnimationSequence(ANIM_JUMP_RIGHT);
      }
    }
    else {
      if (m_eLastDirection == LD_LEFT) {
	changeCurrentAnimationSequence(ANIM_FALL_LEFT);
      }
      else {
	changeCurrentAnimationSequence(ANIM_FALL_RIGHT);
      }
    }
  }

#ifdef DEBUG_CHARACTER_BOUNDING_BOXES
  CDebugDrawer::getSingleton().draw(getWorldBoundingBox());
#endif

  CAnimatedSprite::update(tpf);
}
bool CPlayer::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_RIGHT) {
    m_bRightPressed = true;
  }
  else if (arg.key == OIS::KC_LEFT) {
    m_bLeftPressed = true;
  }
  else if (arg.key == OIS::KC_UP) {
    m_bJumpPressed = true;

    if (m_bOnGround) {
      m_vCurrentSpeed.y = m_fInitialJumpSpeed;
      m_bOnGround = false;
      m_bJumping = true;
    }
  }
  else if (arg.key == OIS::KC_SPACE) {
    m_fBombThrowStrength = PLAYER_BOMB_DEFAULT_THROW_STRENGTH;
    if (m_bOnGround) {
      m_bAttackPressed = true;
    }
  }
  else if (arg.key == OIS::KC_RETURN) {
    m_pMap->activateSwitchOnHit(getWorldBoundingBox());
  }
  return true;
}
bool CPlayer::keyReleased( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_RIGHT) {
    m_bRightPressed = false;
  }
  else if (arg.key == OIS::KC_LEFT) {
    m_bLeftPressed = false;
  }
  else if (arg.key == OIS::KC_UP) {
    m_bJumpPressed = false;
  }
  else if (arg.key ==OIS::KC_SPACE) {
    m_bAttackPressed = false;
  }
  return true;
}
void CPlayer::animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText) {
  if (uiOldText == 3 && uiNewText == 4) {
    if (m_uiCurrentAnimationSequence == ANIM_ATTACK_LEFT) {
      m_pMap->addShot(new CShot(m_pMap, m_pSpriteManager, getCenter() + PLAYER_BOLT_OFFSET_LEFT, CShot::ST_BOLT, CShot::SD_LEFT))->launch(Ogre::Vector2(-1,0));
    }
    else if(m_uiCurrentAnimationSequence == ANIM_ATTACK_RIGHT) {
      m_pMap->addShot(new CShot(m_pMap, m_pSpriteManager, getCenter() + PLAYER_BOLT_OFFSET_RIGHT, CShot::ST_BOLT, CShot::SD_RIGHT))->launch(Ogre::Vector2(1,0));
    }
  }
  else if (uiOldText == m_AnimationSequences[m_uiCurrentAnimationSequence].size() - 1 && uiNewText == 0) {
    // new loop of animation
    if (m_uiCurrentAnimationSequence == ANIM_THROW_LEFT) {
      changeCurrentAnimationSequence(ANIM_STAND_LEFT);
    }
    else if (m_uiCurrentAnimationSequence == ANIM_THROW_RIGHT) {
      changeCurrentAnimationSequence(ANIM_STAND_RIGHT);
    }
  }
}
