#include "Player.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Constants.hpp"
#include "DebugDrawer.hpp"
#include "Shot.hpp"
#include "BarIndicator.hpp"
#include "HUD.hpp"

const Ogre::Vector2 PLAYER_BOLT_OFFSET_RIGHT(0.2, 0.0);
const Ogre::Vector2 PLAYER_BOLT_OFFSET_LEFT(PLAYER_BOLT_OFFSET_RIGHT * Ogre::Vector2(-1, 1));

const Ogre::Vector2 PLAYER_BOMB_OFFSET(0, 0.9);
const Ogre::Vector2 PLAYER_BOMB_THROW_DIRECTION_RIGHT(0.9, 0.1);
const Ogre::Vector2 PLAYER_BOMB_THROW_DIRECTION_LEFT(PLAYER_BOMB_THROW_DIRECTION_RIGHT * Ogre::Vector2(-1, 1));
const Ogre::Real PLAYER_BOMB_DEFAULT_THROW_STRENGTH(0.3);
const Ogre::Real PLAYER_BOMB_THROW_STRENGTH_INCREASE(3);
const Ogre::Real PLAYER_BOMB_MAX_TRHOW_STRENGTH(5);

const Ogre::Real PLAYER_LINK_FADE_TIME(1);

const Ogre::Real PLAYER_MAX_MANA_POINTS(50);
const Ogre::Real PLAYER_BOLT_MANA_COSTS(5);
const Ogre::Real PLAYER_MANA_REGAIN_PER_SEC(0.5);

CPlayer::CPlayer(CMap *pMap, Ogre2dManager *pSpriteManager)
 :
  CAnimatedSprite(pMap, pSpriteManager, Ogre::Vector2(0, 0), Ogre::Vector2(1, 2)),
  CHitableObject(10),
  m_Fader(this),
  m_pMap(pMap),
  m_bLeftPressed(false),
  m_bRightPressed(false),
  m_bAttackPressed(false),
  m_bJumpPressed(false),
  m_bActivateLinkPressed(false),
  m_fMaxWalkSpeed(4),
  m_fInitialJumpSpeed(10),
  m_vCurrentSpeed(0, 0),
  m_bOnGround(true),
  m_bJumping(false),
  m_eLastDirection(LD_RIGHT),
  m_uiCurrentWeapon(W_BOLT),
  m_pBomb(NULL),
  m_Shield(pMap, pSpriteManager, Ogre::Vector2::ZERO, Ogre::Vector2(2, 2)),
  m_bShieldActive(false),
  m_eGoToLinkStatus(GTLS_NONE),
  m_fManaPoints(PLAYER_MAX_MANA_POINTS) {

  CInputListenerManager::getSingleton().addInputListener(this);
  init(1, ANIM_COUNT);
  setupAnimations();
  m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.2, 0));
  m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.6, 1.8));
  m_pThrowStrengthIndicator = new CBarIndicator(pMap, pSpriteManager);

  m_Shield.init(1, 1);
  m_Shield.setupAnimation(0, "shield", 5, CSpriteTexture::MIRROR_NONE, &getPlayerTexturePath);
}
CPlayer::~CPlayer() {
  delete m_pThrowStrengthIndicator;
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
  // mana regain
  m_fManaPoints = min(m_fManaPoints + tpf * PLAYER_MANA_REGAIN_PER_SEC, PLAYER_MAX_MANA_POINTS);
  CHUD::getSingleton().setMP(m_fManaPoints / PLAYER_MAX_MANA_POINTS);

  // ========================================================================
  // Move the player
  // ========================================================================
  if (m_eGoToLinkStatus == GTLS_NONE) {
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

      if (m_pMap->collidesWithMapMargin(getWorldBoundingBox())) {
	m_vPosition.x -= m_vCurrentSpeed.x * tpf;
	m_vCurrentSpeed.x = 0;
      }
    }
    // Jump if key pressed and on ground
    if (m_bJumpPressed && m_bOnGround) {
      m_vCurrentSpeed.y = m_fInitialJumpSpeed;
      m_bOnGround = false;
      m_bJumping = true;
    }

    // if activate link and on ground
    if (m_bActivateLinkPressed && m_bOnGround) {
      if (m_pMap->findLink(getWorldBoundingBox(), m_vLinkFromPos, m_vLinkToPos)) {
	m_eGoToLinkStatus = GTLS_MOVE_TO_ENTRANCE;
	m_Fader.startFadeOut(PLAYER_LINK_FADE_TIME);
      }
    }
  }
  else if (m_eGoToLinkStatus == GTLS_MOVE_TO_ENTRANCE) {
    Ogre::Real fOldDistanceSq = m_vPosition.squaredDistance(m_vLinkFromPos);
    Ogre::Vector2 vDir = (m_vLinkFromPos - m_vPosition).normalisedCopy();
    m_vPosition += vDir * m_fMaxWalkSpeed * tpf;
    if (fOldDistanceSq <= m_vPosition.squaredDistance(m_vLinkFromPos)) {
      m_vPosition = m_vLinkFromPos;
    }
  }
  else if (m_eGoToLinkStatus == GTLS_COME_OUT_FROM_EXIT) {
  }

  // ========================================================================
  // Update animations
  // ========================================================================

  if (m_uiCurrentAnimationSequence == ANIM_HOLD_LEFT || m_uiCurrentAnimationSequence == ANIM_HOLD_RIGHT) {
    if (!m_bAttackPressed) {
      // release bomb
      m_pBomb->launch(((m_eLastDirection == LD_LEFT) ? PLAYER_BOMB_THROW_DIRECTION_LEFT : PLAYER_BOMB_THROW_DIRECTION_RIGHT) * m_fBombThrowStrength);
      m_pBomb = NULL;
      m_pThrowStrengthIndicator->hide();
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
	  m_pThrowStrengthIndicator->show();
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

  m_pThrowStrengthIndicator->setCenter(getCenter() + 0.5 * getSize().y * Ogre::Vector2::UNIT_Y);
  m_pThrowStrengthIndicator->setValue(m_fBombThrowStrength / PLAYER_BOMB_MAX_TRHOW_STRENGTH);
  m_pThrowStrengthIndicator->update(tpf);

  m_Fader.fade(tpf);

  CAnimatedSprite::update(tpf);
  if (m_bShieldActive) {
    m_Shield.setCenter(getCenter());
    m_Shield.update(tpf);
  }
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
  }
  else if (arg.key == OIS::KC_DOWN) {
    m_bActivateLinkPressed = true;
  }
  else if (arg.key == OIS::KC_SPACE) {
    if (m_uiCurrentWeapon == W_BOMB) {
      m_fBombThrowStrength = PLAYER_BOMB_DEFAULT_THROW_STRENGTH;
    }
    else if (m_uiCurrentWeapon == W_SHIELD) {
      m_bShieldActive = !m_bShieldActive;
    }

    if (m_bOnGround) {
      m_bAttackPressed = true;
    }
  }
  else if (arg.key == OIS::KC_RETURN) {
    m_pMap->activateSwitchOnHit(getWorldBoundingBox());
  }
  else if (arg.key == OIS::KC_COMMA) {
    if (m_uiCurrentWeapon == 0) {
      m_uiCurrentWeapon = W_COUNT - 1;
    }
    else {
      --m_uiCurrentWeapon;
    }
    CHUD::getSingleton().setCurrentWeapon(m_uiCurrentWeapon);
  }
  else if (arg.key == OIS::KC_PERIOD) {
    ++m_uiCurrentWeapon;
    if (m_uiCurrentWeapon >= W_COUNT) {
      m_uiCurrentWeapon = 0;
    }
    CHUD::getSingleton().setCurrentWeapon(m_uiCurrentWeapon);
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
  else if (arg.key == OIS::KC_DOWN) {
    m_bActivateLinkPressed = false;
  }
  return true;
}
void CPlayer::animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText) {
  if (uiOldText == 3 && uiNewText == 4) {
    if (m_uiCurrentAnimationSequence == ANIM_ATTACK_RIGHT || m_uiCurrentAnimationSequence == ANIM_ATTACK_LEFT) {
      if (m_fManaPoints < PLAYER_BOLT_MANA_COSTS) {
	return;
      }
      m_fManaPoints -= PLAYER_BOLT_MANA_COSTS;

      if (m_uiCurrentAnimationSequence == ANIM_ATTACK_LEFT) {
	m_pMap->addShot(new CShot(m_pMap, m_pSpriteManager, getCenter() + PLAYER_BOLT_OFFSET_LEFT, CShot::ST_BOLT, CShot::SD_LEFT))->launch(Ogre::Vector2(-1,0));
      }
      else if(m_uiCurrentAnimationSequence == ANIM_ATTACK_RIGHT) {
	m_pMap->addShot(new CShot(m_pMap, m_pSpriteManager, getCenter() + PLAYER_BOLT_OFFSET_RIGHT, CShot::ST_BOLT, CShot::SD_RIGHT))->launch(Ogre::Vector2(1,0));
      }
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
void CPlayer::fadeInCallback() {
  if (m_eGoToLinkStatus == GTLS_COME_OUT_FROM_EXIT) {
    m_eGoToLinkStatus = GTLS_NONE;
  }

}
void CPlayer::fadeOutCallback() {
  if (m_eGoToLinkStatus == GTLS_MOVE_TO_ENTRANCE) {
    m_vPosition = m_vLinkToPos;
    m_eGoToLinkStatus = GTLS_COME_OUT_FROM_EXIT;
    m_Fader.startFadeIn(PLAYER_LINK_FADE_TIME);
  }
}

void CPlayer::damageTakenCallback() {
  CHUD::getSingleton().setHP(getHitpoints() / getMaximumHitpoints());
}

void CPlayer::killedByDamageCallback() {
}
