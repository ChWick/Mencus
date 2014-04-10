#include "Player.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "Constants.hpp"
#include "DebugDrawer.hpp"
#include "Shot.hpp"
#include "BarIndicator.hpp"
#include "HUD.hpp"
#include "Object.hpp"
#include "CheatDefines.hpp"
#include "GameState.hpp"
#include "GameInputCommand.hpp"
#include "XMLHelper.hpp"
#include "Statistics.hpp"
#include "GUIStatistics.hpp"

using namespace XMLHelper;
using namespace Weapon;

unsigned int PLAYER_LINK_PAUSE = PAUSE_ENEMY_MOVEMENT | PAUSE_SHOT_MOVEMENT;

const Ogre::Vector2 PLAYER_BOLT_OFFSET_RIGHT(0.2, 0.125);
const Ogre::Vector2 PLAYER_BOLT_OFFSET_LEFT(PLAYER_BOLT_OFFSET_RIGHT * Ogre::Vector2(-1, 1));

const Ogre::Vector2 PLAYER_COLUMN_OFFSET_RIGHT(0.2, 0.0);
const Ogre::Vector2 PLAYER_COLUMN_OFFSET_LEFT(PLAYER_BOLT_OFFSET_RIGHT * Ogre::Vector2(-1, 1));

const Ogre::Vector2 PLAYER_BOMB_OFFSET(0, 0.9);
const Ogre::Vector2 PLAYER_BOMB_THROW_DIRECTION_RIGHT(0.9, 0.1);
const Ogre::Vector2 PLAYER_BOMB_THROW_DIRECTION_LEFT(PLAYER_BOMB_THROW_DIRECTION_RIGHT * Ogre::Vector2(-1, 1));
const Ogre::Real PLAYER_BOMB_DEFAULT_THROW_STRENGTH(0.3);
const Ogre::Real PLAYER_BOMB_THROW_STRENGTH_INCREASE(3);
const Ogre::Real PLAYER_BOMB_MAX_TRHOW_STRENGTH(5);

const Ogre::Real PLAYER_LINK_FADE_TIME(1);

const Ogre::Real PLAYER_MAX_MANA_POINTS(50);
const Ogre::Real PLAYER_BOLT_MANA_COSTS(5);
const Ogre::Real PLAYER_COLUMN_MANA_COSTS(2);
const Ogre::Real PLAYER_SHIELD_MANA_COSTS_PER_SEC(5);
const Ogre::Real PLAYER_MANA_REGAIN_PER_SEC(0.5);

const Ogre::Real PLAYER_HEALTH_POTION_REGAIN_PERCENTAGE(0.5f);
const Ogre::Real PLAYER_MANA_POTION_REGAIN_PERCENTAGE(0.5f);

const Ogre::Real SPIKES_DAMANGE_PER_HIT(1);

CPlayer::CPlayer(CMap &map, SStatistics &statistics)
  :
  CAnimatedSprite(map, "Player", &map, &map, map.get2dManager(), Ogre::Vector2(0, 0), Ogre::Vector2(1, 2)),
  CHitableObject(10),
  m_Fader(this),
  m_fRightPressed(0),
  m_fLeftPressed(0),
  m_bAttackPressed(false),
  m_bJumpPressed(false),
  m_bActivateLinkPressed(false),
  m_fMaxWalkSpeed(4),
  m_fInitialJumpSpeed(11.5),
  m_vCurrentSpeed(0, 0),
  m_bOnGround(true),
  m_bJumping(false),
  m_eLastDirection(LD_RIGHT),
  m_uiCurrentWeapon(W_BOLT),
  m_pBomb(NULL),
  m_Shield(map, "Shield", this, &map, map.get2dManager(), Ogre::Vector2::ZERO, Ogre::Vector2(2, 2)),
  m_bShieldActive(false),
  m_eGoToLinkStatus(GTLS_NONE),
  m_fManaPoints(PLAYER_MAX_MANA_POINTS),
  m_uiKeyCount(0),
  m_uiHealthPotionsCount(0),
  m_uiManaPotionsCount(0),
  m_uiBombCount(0),
  m_Statistics(statistics) {
  constructor_impl();
}
CPlayer::CPlayer(CMap &map, const tinyxml2::XMLElement *pElem, SStatistics &statistics) 
  : CAnimatedSprite(map, &map, &map, map.get2dManager(), pElem, Ogre::Vector2(1, 2)),
    CHitableObject(pElem),
    m_Fader(this),
    m_fRightPressed(0),
    m_fLeftPressed(0),
    m_bAttackPressed(false),
    m_bJumpPressed(false),
    m_bActivateLinkPressed(false),
    m_fMaxWalkSpeed(4),
    m_fInitialJumpSpeed(11.5),
    m_vCurrentSpeed(Vector2Attribute(pElem, "speed", Ogre::Vector2::ZERO)),
    m_bOnGround(true),
    m_bJumping(false),
    m_eLastDirection(LD_RIGHT),
    m_uiCurrentWeapon(IntAttribute(pElem, "pl_cur_weapon", W_BOLT)),
    m_pBomb(NULL),
    m_fBombThrowStrength(RealAttribute(pElem, "pl_bomb_throw_str", 0)),
  m_Shield(map, "Shield", this, &map, map.get2dManager(), Ogre::Vector2::ZERO, Ogre::Vector2(2, 2)),
  m_bShieldActive(BoolAttribute(pElem, "pl_shield_active", false)),
  m_vLinkFromPos(Vector2Attribute(pElem, "pl_gtl_from", Ogre::Vector2::ZERO)),
  m_vLinkToPos(Vector2Attribute(pElem, "pl_gtl_to", Ogre::Vector2::ZERO)),
  m_eGoToLinkStatus(EnumAttribute<EGoToLinkStatus>(pElem, "pl_gtl_status", GTLS_NONE)),
  m_fManaPoints(RealAttribute(pElem, "pl_hud_mp", PLAYER_MAX_MANA_POINTS)),
  m_uiKeyCount(IntAttribute(pElem, "pl_hud_key", 0)),
  m_uiHealthPotionsCount(IntAttribute(pElem, "pl_hud_hp_cnt", 0)),
  m_uiManaPotionsCount(IntAttribute(pElem, "pl_hud_mp_cnt", 0)),
  m_uiBombCount(IntAttribute(pElem, "pl_hud_bomb_cnt", 0)),
  m_Statistics(statistics) {
  constructor_impl();
  startup(getPosition(), RealAttribute(pElem, "direction", 1));
}
void CPlayer::constructor_impl() {
  setID("Player");

#ifdef DEBUG_PLAYER_NO_COLLISION
  m_bPlayerNoCollisionActivated = false;
#endif // DEBUG_PLAYER_NO_COLLISION

  CInputListenerManager::getSingleton().addInputListener(this);
  CAnimatedSprite::init(1, ANIM_COUNT);
  CHitableObject::init();
  setupAnimations();
  m_bbRelativeBoundingBox.setPosition(Ogre::Vector2(0.2, 0));
  m_bbRelativeBoundingBox.setSize(Ogre::Vector2(0.6, 1.8));
  m_pThrowStrengthIndicator = new CBarIndicator(m_Map, this, &m_Map, m_Map.get2dManager());

  m_Shield.init(1, 1);
  m_Shield.setupAnimation(0, "shield", 5, CSpriteTexture::MIRROR_NONE, &getPlayerTexturePath);

  // initialize hud
  CHUD::getSingleton().setCurrentWeapon(m_uiCurrentWeapon);
  CHUD::getSingleton().setHP(getHitpoints() / getMaximumHitpoints());
  CHUD::getSingleton().setMP(m_fManaPoints / PLAYER_MAX_MANA_POINTS);
  CHUD::getSingleton().setBombCount(m_uiBombCount);
  CHUD::getSingleton().setManaPotionCount(m_uiManaPotionsCount);
  CHUD::getSingleton().setHealthPotionCount(m_uiHealthPotionsCount);
  CHUD::getSingleton().setKeysCount(m_uiKeyCount);
}
CPlayer::~CPlayer() {
  delete m_pThrowStrengthIndicator;
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CPlayer::startup(const Ogre::Vector2 &vPosition, Ogre::Real fDirection) {
  setPosition(vPosition);
  m_eLastDirection = (fDirection > 0) ? LD_RIGHT : LD_LEFT;
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
    m_vCurrentSpeed.x = m_fMaxWalkSpeed * (m_fRightPressed - m_fLeftPressed);
    m_vCurrentSpeed.y += c_fGravity * tpf;

    if (m_vCurrentSpeed.y > 0 && !m_bJumpPressed) {
      // If the user wants to cancel the jump, do a higher acceleration
      m_vCurrentSpeed.y += c_fGravity * tpf; // twice gravity now
    }

    if (!m_pBomb) {
      Ogre::Real fPenetration = 0;
      // move and check here only y direction movement
      m_vPosition.y += m_vCurrentSpeed.y * tpf;

      CTile *pTile(NULL);
      if (m_vCurrentSpeed.y < 0) {
        m_bOnGround = false;
        m_bJumping = true;
        fPenetration = m_Map.hitsTile(CCD_BOTTOM, CTile::TF_UNPASSABLE, getWorldBoundingBox(), &pTile);
        if (fPenetration != 0) {
          m_bOnGround = true;
          m_bJumping = false;
        }
      } else if (m_vCurrentSpeed.y > 0) {
        fPenetration = m_Map.hitsTile(CCD_TOP, CTile::TF_UNPASSABLE, getWorldBoundingBox(), &pTile);
      }
      if (fPenetration == 0) {
        if (m_Map.outOfMap(getWorldBoundingBox(), CCD_VERTICAL)) {
          fPenetration = m_vCurrentSpeed.y * tpf;
        }
      }

#ifdef DEBUG_PLAYER_NO_COLLISION
      if (m_bPlayerNoCollisionActivated) {
        fPenetration = 0.f;
      }
#endif // DEBUG_PLAYER_NO_COLLISION
      if (fPenetration != 0) {
        m_vCurrentSpeed.y = 0;
        m_vPosition.y -= fPenetration;
      }
      if (pTile && (pTile->getTileFlags() & CTile::TF_DAMAGES)) {
        // last hope check if the tile right of the damage tile hits the player 2 and is not damaging him
        CTile *pRightTile = m_Map.getTile(static_cast<int>(getWorldBoundingBox().getRight() - 0.01), pTile->getMapPosY());
        if (pRightTile && pRightTile->getWorldBoundingBox().collidesWith(getWorldBoundingBox()) != CCD_NONE && (pRightTile->getTileFlags() & CTile::TF_DAMAGES) == 0) {

        }
        else {
          takeDamage(SPIKES_DAMANGE_PER_HIT);
          m_bOnGround = false;
          m_bJumping = true;
          m_vCurrentSpeed.y = m_fInitialJumpSpeed;
        }
      }

      // move and check here only x direction movement
      fPenetration = 0;
      m_vPosition.x += m_vCurrentSpeed.x * tpf;

      if (m_vCurrentSpeed.x < 0) {
        fPenetration = m_Map.hitsTile(CCD_LEFT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
        m_eLastDirection = LD_LEFT;
      } else if (m_vCurrentSpeed.x > 0) {
        fPenetration += m_Map.hitsTile(CCD_RIGHT, CTile::TF_UNPASSABLE, getWorldBoundingBox());
        m_eLastDirection = LD_RIGHT;
      }

      if (fPenetration == 0) {
        if (m_Map.outOfMap(getWorldBoundingBox(), CCD_HORIZONTAL)) {
          fPenetration = m_vCurrentSpeed.x * tpf;
        }
      }

#ifdef DEBUG_PLAYER_NO_COLLISION
      if (m_bPlayerNoCollisionActivated) {
        fPenetration = 0.f;
      }
#endif // DEBUG_PLAYER_NO_COLLISION

      if (fPenetration != 0) {
        // check if collision with door
        if (m_uiKeyCount > 0) {
          Ogre::Real fLockPenetration = 0;
          CTile *pTile(NULL);
          if (m_vCurrentSpeed.x < 0) {
            fLockPenetration = m_Map.hitsTile(CCD_LEFT, CTile::TF_LOCK, getWorldBoundingBox(), &pTile);
          } else if (m_vCurrentSpeed.x > 0) {
            fLockPenetration += m_Map.hitsTile(CCD_RIGHT, CTile::TF_LOCK, getWorldBoundingBox(), &pTile);
          }
          if (fLockPenetration != 0) {
            m_uiKeyCount--;
	    CHUD::getSingleton().setKeysCount(m_uiKeyCount);
	    m_Statistics.uiUsedItems[Weapon::I_KEY]++;
            m_Map.unlock(pTile->getMapPosX(), pTile->getMapPosY());
          }
        }
        m_vPosition.x -= fPenetration;
        m_vCurrentSpeed.x = 0;
      }

      if (m_Map.collidesWithMapMargin(getWorldBoundingBox())) {
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
      if (m_Map.findLink(getWorldBoundingBox(), m_vLinkFromPos, m_vLinkToPos)) {
        m_eGoToLinkStatus = GTLS_MOVE_TO_ENTRANCE;
        m_Fader.startFadeOut(PLAYER_LINK_FADE_TIME);
        pause(PLAYER_LINK_PAUSE);
      }
    }
  } else if (m_eGoToLinkStatus == GTLS_MOVE_TO_ENTRANCE) {
    Ogre::Real fOldDistanceSq = m_vPosition.squaredDistance(m_vLinkFromPos);
    Ogre::Vector2 vDir = (m_vLinkFromPos - m_vPosition).normalisedCopy();
    m_vPosition += vDir * m_fMaxWalkSpeed * tpf;
    if (fOldDistanceSq <= m_vPosition.squaredDistance(m_vLinkFromPos)) {
      m_vPosition = m_vLinkFromPos;
    }
  } else if (m_eGoToLinkStatus == GTLS_COME_OUT_FROM_EXIT) {
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
      } else {
        changeCurrentAnimationSequence(ANIM_THROW_RIGHT);
      }
    } else {
      // increase throw strength
      m_fBombThrowStrength += PLAYER_BOMB_THROW_STRENGTH_INCREASE * tpf;
      m_fBombThrowStrength = min(m_fBombThrowStrength, PLAYER_BOMB_MAX_TRHOW_STRENGTH);
    }
  } else if (m_uiCurrentAnimationSequence == ANIM_THROW_RIGHT || m_uiCurrentAnimationSequence == ANIM_THROW_LEFT) {
    // do nothing, wait for finished
  } else if (m_bOnGround) {
    if (m_bAttackPressed && m_vCurrentSpeed.x == 0) {
      if (m_uiCurrentWeapon == W_BOLT || m_uiCurrentWeapon == W_COLUMN) {
        if (m_eLastDirection == LD_LEFT) {
          changeCurrentAnimationSequence(ANIM_ATTACK_LEFT);
        } else {
          changeCurrentAnimationSequence(ANIM_ATTACK_RIGHT);
        }
      } else if (m_uiCurrentWeapon == W_BOMB) {
        if (!m_pBomb) {
          m_pThrowStrengthIndicator->show();
          m_pBomb = new CShot(m_Map, getCenter(), CShot::ST_BOMB, (m_eLastDirection == LD_LEFT) ? CShot::SD_LEFT : CShot::SD_RIGHT);
          m_uiBombCount--;
	  m_Statistics.uiUsedItems[Weapon::I_BOMB]++;
          CHUD::getSingleton().setBombCount(m_uiBombCount);
        }
        m_pBomb->setCenter(getCenter() + PLAYER_BOMB_OFFSET);
        if (m_eLastDirection == LD_LEFT) {
          changeCurrentAnimationSequence(ANIM_HOLD_LEFT);
        } else {
          changeCurrentAnimationSequence(ANIM_HOLD_RIGHT);
        }
      }
    } else {
      if (m_vCurrentSpeed.x < 0) {
        changeCurrentAnimationSequence(ANIM_WALK_LEFT);
      } else if (m_vCurrentSpeed.x > 0) {
        changeCurrentAnimationSequence(ANIM_WALK_RIGHT);
      } else {
        if (m_eLastDirection == LD_LEFT) {
          changeCurrentAnimationSequence(ANIM_STAND_LEFT);
        } else {
          changeCurrentAnimationSequence(ANIM_STAND_RIGHT);
        }
      }
    }
  } else if (m_bJumping) {
    if (m_vCurrentSpeed.y > -5.0) { // fall animation appears at a certain fall speed
      if (m_eLastDirection == LD_LEFT) {
        changeCurrentAnimationSequence(ANIM_JUMP_LEFT);
      } else {
        changeCurrentAnimationSequence(ANIM_JUMP_RIGHT);
      }
    } else {
      if (m_eLastDirection == LD_LEFT) {
        changeCurrentAnimationSequence(ANIM_FALL_LEFT);
      } else {
        changeCurrentAnimationSequence(ANIM_FALL_RIGHT);
      }
    }
  }

  m_pThrowStrengthIndicator->setCenter(getCenter() + 0.5 * getSize().y * Ogre::Vector2::UNIT_Y);
  m_pThrowStrengthIndicator->setValue(m_fBombThrowStrength / PLAYER_BOMB_MAX_TRHOW_STRENGTH);
  m_pThrowStrengthIndicator->update(tpf);

  m_Fader.fade(tpf);

  CAnimatedSprite::update(tpf);
  if (m_bShieldActive) {
    m_fManaPoints -= PLAYER_SHIELD_MANA_COSTS_PER_SEC * tpf;
    m_Statistics.fUsedManapoints += PLAYER_SHIELD_MANA_COSTS_PER_SEC * tpf;;
    if (m_fManaPoints <= 0) {
      m_bShieldActive = 0;
      m_fManaPoints = 0;
    } else {
      m_Shield.setCenter(getCenter());
      m_Shield.update(tpf);
    }
  }

  setInvunerable(m_bShieldActive);


  m_Statistics.fHitpoints = getHitpoints();
  m_Statistics.fManapoints = getManapoints();
}
void CPlayer::render(Ogre::Real tpf) {
  CAnimatedSprite::render(tpf);

#ifdef DEBUG_CHARACTER_BOUNDING_BOXES
  CDebugDrawer::getSingleton().draw(getWorldBoundingBox());
#endif
  m_pThrowStrengthIndicator->render(tpf);
  if (m_bShieldActive) {
    m_Shield.render(tpf);
  }
}
void CPlayer::pickobject(unsigned int uiObjectId) {
  switch (uiObjectId) {
  case CObject::OT_BOMB:
    m_uiBombCount++;
    CHUD::getSingleton().setBombCount(m_uiBombCount);
    break;
  case CObject::OT_MANA_POTION:
    m_uiManaPotionsCount++;
    CHUD::getSingleton().setManaPotionCount(m_uiManaPotionsCount);
    break;
  case CObject::OT_HEALTH_POTION:
    ++m_uiHealthPotionsCount;
    CHUD::getSingleton().setHealthPotionCount(m_uiHealthPotionsCount);
    break;
  case CObject::OT_KEY:
    ++m_uiKeyCount;
    CHUD::getSingleton().setKeysCount(m_uiKeyCount);
    break;
  default:
    break;
  }
}
bool CPlayer::keyPressed( const OIS::KeyEvent &arg ) {
 if (arg.key == OIS::KC_COMMA) {
    if (m_uiCurrentWeapon == 0) {
      m_uiCurrentWeapon = W_COUNT - 1;
    } else {
      --m_uiCurrentWeapon;
    }
    CHUD::getSingleton().setCurrentWeapon(m_uiCurrentWeapon);
  } else if (arg.key == OIS::KC_PERIOD) {
    ++m_uiCurrentWeapon;
    if (m_uiCurrentWeapon >= W_COUNT) {
      m_uiCurrentWeapon = 0;
    }
    CHUD::getSingleton().setCurrentWeapon(m_uiCurrentWeapon);
  }
#ifdef CHEAT_ADD_KEYS
  else if (arg.key == OIS::KC_F7) {
    ++m_uiKeyCount;
    CHUD::getSingleton().setKeysCount(m_uiKeyCount);
  }
#endif // CHEAT_ADD_KEYS
#ifdef CHEAT_ADD_HEALTH_POTIONS
  else if (arg.key == OIS::KC_F8) {
    ++m_uiHealthPotionsCount;
    CHUD::getSingleton().setHealthPotionCount(m_uiHealthPotionsCount);
  }
#endif // CHEAT_ADD_HEALTH_POTIONS
#ifdef CHEAT_ADD_MANA_POTIONS
  else if (arg.key == OIS::KC_F9) {
    ++m_uiManaPotionsCount;
    CHUD::getSingleton().setManaPotionCount(m_uiManaPotionsCount);
  }
#endif // CHEAT_ADD_MANA_POTIONS
#ifdef CHEAT_ADD_BOMB
  else if (arg.key == OIS::KC_F10) {
    ++m_uiBombCount;
    CHUD::getSingleton().setBombCount(m_uiBombCount);
  }
#endif // CHEAT_ADD_BOMB
#ifdef DEBUG_PLAYER_NO_COLLISION
  else if (arg.key == OIS::KC_F11) {
    m_bPlayerNoCollisionActivated = !m_bPlayerNoCollisionActivated;
  }
#endif // DEBUG_PLAYER_NO_COLLISION
#ifdef CHEAT_MEGA_JUMP
  else if (arg.key == OIS::KC_LCONTROL) {
    m_vCurrentSpeed.y += m_fInitialJumpSpeed;
  }
#endif // CHEAT_MEGA_JUMP

  return true;
}
bool CPlayer::keyReleased( const OIS::KeyEvent &arg ) {
  return true;
}
void CPlayer::receiveInputCommand( const CGameInputCommand &cmd) {
  switch (cmd.getType()) {
  case GIC_LEFT:
    m_fLeftPressed = cmd.getValue();
    break;
  case GIC_RIGHT:
    m_fRightPressed = cmd.getValue();
    break;
  case GIC_JUMP:
    m_bJumpPressed = cmd.getValue() > 0.5;
    break;
  case GIC_ENTER_LINK:
    m_bActivateLinkPressed = cmd.getValue() > 0.5;
    break;
  case GIC_ATTACK:
    if (cmd.getFloatValue() > 0.5) {
      if (m_eGoToLinkStatus == GTLS_NONE) {
	if (m_uiCurrentWeapon == W_BOMB) {
	  if (m_uiBombCount <= 0) {
	    break;
	  }
	  if (cmd.getState() == GIS_PRESSED) {
	    m_fBombThrowStrength = PLAYER_BOMB_DEFAULT_THROW_STRENGTH;
	  }
	} else if (m_uiCurrentWeapon == W_SHIELD) {
	  if (cmd.getState() == GIS_PRESSED) {
	    m_bShieldActive = !m_bShieldActive;
	  }
	}

	if (m_bOnGround) {
	  m_bAttackPressed = true;
	}
      }
    }
    else {
      m_bAttackPressed = false;
    }
    break;
  case GIC_ACTIVATE:
    if (cmd.getState() == GIS_PRESSED) {
      m_Map.activateSwitchOnHit(getWorldBoundingBox());
    }
    break;
  case GIC_USE_HEALTH_POTION:
    if (m_uiHealthPotionsCount > 0 && (cmd.getState() == GIS_PRESSED || cmd.getState() == GIS_CLICKED)) {
      --m_uiHealthPotionsCount;
      addHitpoints(PLAYER_HEALTH_POTION_REGAIN_PERCENTAGE * getMaximumHitpoints());
      CHUD::getSingleton().setHealthPotionCount(m_uiHealthPotionsCount);
      CHUD::getSingleton().setHP(getHitpoints() / getMaximumHitpoints());
      m_Statistics.uiUsedItems[Weapon::I_HEALTH_POTION]++;
    }
    break;
  case GIC_USE_MANA_POTION:
    if (m_uiManaPotionsCount > 0 && (cmd.getState() == GIS_PRESSED || cmd.getState() == GIS_CLICKED)) {
      --m_uiManaPotionsCount;
      m_fManaPoints = min(PLAYER_MAX_MANA_POINTS, m_fManaPoints + PLAYER_MANA_POTION_REGAIN_PERCENTAGE * PLAYER_MAX_MANA_POINTS);
      CHUD::getSingleton().setManaPotionCount(m_uiManaPotionsCount);
      CHUD::getSingleton().setMP(m_fManaPoints / PLAYER_MAX_MANA_POINTS);
      m_Statistics.uiUsedItems[Weapon::I_MANA_POTION]++;
    }
    break;
  case GIC_CHANGE_WEAPON:
    m_uiCurrentWeapon = min(W_COUNT - 1, max(0, cmd.getIntValue()));
    CHUD::getSingleton().setCurrentWeapon(m_uiCurrentWeapon);
    break;
  default:
    break;
  }
}
void CPlayer::animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText) {
  if (uiOldText == 3 && uiNewText == 4) {
    if (m_uiCurrentAnimationSequence == ANIM_ATTACK_RIGHT || m_uiCurrentAnimationSequence == ANIM_ATTACK_LEFT) {
      Ogre::Real fManaCosts = PLAYER_BOLT_MANA_COSTS;
      if (m_uiCurrentWeapon == W_COLUMN) {
        fManaCosts = PLAYER_COLUMN_MANA_COSTS;
      }
      if (m_fManaPoints < fManaCosts) {
        return;
      }
      m_fManaPoints -= fManaCosts;
      m_Statistics.fUsedManapoints += fManaCosts;

      CShot::EShotTypes eShotType = CShot::ST_BOLT;
      Ogre::Vector2 vOffset = getPosition();
      Ogre::Vector2 vLaunchDirection;
      CShot::EShotDirections eShotDir;
      if (m_uiCurrentAnimationSequence == ANIM_ATTACK_LEFT) {
        vLaunchDirection = Ogre::Vector2(-1, 0);
        eShotDir = CShot::SD_LEFT;
      }
      else {
        vLaunchDirection = Ogre::Vector2(1, 0);
        eShotDir = CShot::SD_RIGHT;
      }

      if (m_uiCurrentWeapon == W_COLUMN) {
        eShotType = CShot::ST_COLUMN;
        if (m_uiCurrentAnimationSequence == ANIM_ATTACK_LEFT) {vOffset = getCenter() + PLAYER_COLUMN_OFFSET_LEFT;}
        else if (m_uiCurrentAnimationSequence == ANIM_ATTACK_RIGHT) {vOffset = getCenter() + PLAYER_COLUMN_OFFSET_RIGHT;}
      }
      else if (m_uiCurrentWeapon == W_BOLT) {
        eShotType = CShot::ST_BOLT;
        if (m_uiCurrentAnimationSequence == ANIM_ATTACK_LEFT) {vOffset = getCenter() + PLAYER_BOLT_OFFSET_LEFT;}
        else if (m_uiCurrentAnimationSequence == ANIM_ATTACK_RIGHT) {vOffset = getCenter() + PLAYER_BOLT_OFFSET_RIGHT;}
      }

      CShot *pShot = new CShot(m_Map, vOffset, eShotType, eShotDir, CShot::DMG_ENEMY);
      pShot->launch(vLaunchDirection);
    }
  } else if (uiOldText == m_AnimationSequences[m_uiCurrentAnimationSequence].size() - 1 && uiNewText == 0) {
    // new loop of animation
    if (m_uiCurrentAnimationSequence == ANIM_THROW_LEFT) {
      changeCurrentAnimationSequence(ANIM_STAND_LEFT);
    } else if (m_uiCurrentAnimationSequence == ANIM_THROW_RIGHT) {
      changeCurrentAnimationSequence(ANIM_STAND_RIGHT);
    }
  }
}
void CPlayer::fadeInCallback() {
  if (m_eGoToLinkStatus == GTLS_COME_OUT_FROM_EXIT) {
    m_eGoToLinkStatus = GTLS_NONE;
    unpause(PLAYER_LINK_PAUSE);
  }

}
void CPlayer::fadeOutCallback() {
  if (m_eGoToLinkStatus == GTLS_MOVE_TO_ENTRANCE) {
    m_vPosition = m_vLinkToPos;
    m_Map.playerWarped();
    m_eGoToLinkStatus = GTLS_COME_OUT_FROM_EXIT;
    m_Fader.startFadeIn(PLAYER_LINK_FADE_TIME);
  }
}

void CPlayer::damageTakenCallback(Ogre::Real fHitpoints) {
  CHUD::getSingleton().setHP(getHitpoints() / getMaximumHitpoints());
  m_Statistics.fLostHitpoints += fHitpoints;
}

void CPlayer::killedByDamageCallback() {
  m_Statistics.eMissionState = MS_FAILED;
  CGUIStatistics::getSingleton().showStatistics(m_Statistics);
  CGameState::getSingleton().changeGameState(CGameState::GS_STATISTICS);
}

void CPlayer::playerInputPauseChanged(bool bPause) {
  setInputListenerEnabled(!bPause);
}

void CPlayer::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  using namespace XMLHelper;

  CAnimatedSprite::writeToXMLElement(pElem, eStyle);
  CHitableObject::writeToXMLElement(pElem, eStyle);

  pElem->SetAttribute("direction", m_eLastDirection == LD_RIGHT ? 1.0 : -1.0);
  if (eStyle == OS_FULL) {
    SetAttribute(pElem, "speed", m_vCurrentSpeed);
    pElem->SetAttribute("pl_cur_weapon", m_uiCurrentWeapon);
    pElem->SetAttribute("pl_bomb_throw_str", m_fBombThrowStrength);
    pElem->SetAttribute("pl_shield_active", m_bShieldActive);
    
    pElem->SetAttribute("pl_gtl_status", m_eGoToLinkStatus);
    SetAttribute<Ogre::Vector2>(pElem, "pl_gtl_from", m_vLinkFromPos);
    SetAttribute<Ogre::Vector2>(pElem, "pl_gtl_to", m_vLinkToPos);
    
    pElem->SetAttribute("pl_hud_mp", m_fManaPoints);
    pElem->SetAttribute("pl_hud_key", m_uiKeyCount);
    pElem->SetAttribute("pl_hud_hp_cnt", m_uiHealthPotionsCount);
    pElem->SetAttribute("pl_hud_mp_cnt", m_uiManaPotionsCount);
    pElem->SetAttribute("pl_hud_bomb_cnt", m_uiBombCount);
  }
}
