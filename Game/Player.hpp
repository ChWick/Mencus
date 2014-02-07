#ifndef PLAYER_H
#define PLAYER_H

#include "AnimatedSprite.hpp"
#include "InputListener.hpp"
#include "Fader.hpp"
#include "HitableObject.hpp"
#include "PauseListener.hpp"
#include "PauseCaller.hpp"
#include "DebugDefines.hpp"

class CMap;
class CShot;
class CBarIndicator;

class CPlayer : public CAnimatedSprite,
		public CInputListener,
		public CFaderCallback,
		public CHitableObject,
		public CPauseListener,
		public CPauseCaller {

private:
  enum ELastDirection {
    LD_LEFT,
    LD_RIGHT
  };
  enum EGoToLinkStatus {
    GTLS_NONE,        		//!< not in a link
    GTLS_MOVE_TO_ENTRANCE,	//!< move to the entrance
    GTLS_COME_OUT_FROM_EXIT,	//!< come out of the exit
  };

private:
  enum EWeapons {
    W_BOLT,
    W_BOMB,
    W_SHIELD,
    W_COLUMN,
    W_COUNT,
  };

  enum EPlayerAnimations {
    ANIM_STAND_RIGHT,
    ANIM_STAND_LEFT,
    ANIM_WALK_RIGHT,
    ANIM_WALK_LEFT,
    ANIM_ATTACK_RIGHT,
    ANIM_ATTACK_LEFT,
    ANIM_JUMP_RIGHT,
    ANIM_JUMP_LEFT,
    ANIM_FALL_RIGHT,
    ANIM_FALL_LEFT,
    ANIM_HOLD_RIGHT,
    ANIM_HOLD_LEFT,
    ANIM_THROW_RIGHT,
    ANIM_THROW_LEFT,
    ANIM_COLUMN_RIGHT,
    ANIM_COLUMN_LEFT,

    ANIM_COUNT
  };

private:
  CFader m_Fader;
  CMap *m_pMap;
  bool m_bRightPressed;
  bool m_bLeftPressed;
  bool m_bAttackPressed;
  bool m_bJumpPressed;
  bool m_bActivateLinkPressed;
  Ogre::Real m_fMaxWalkSpeed;
  Ogre::Real m_fInitialJumpSpeed;
  Ogre::Vector2 m_vCurrentSpeed;

  bool m_bOnGround;
  bool m_bJumping;
  ELastDirection m_eLastDirection;
  unsigned int m_uiCurrentWeapon;
  CShot *m_pBomb;
  Ogre::Real m_fBombThrowStrength;
  CBarIndicator *m_pThrowStrengthIndicator;
  CAnimatedSprite m_Shield;
  bool m_bShieldActive;

  Ogre::Vector2 m_vLinkFromPos;
  Ogre::Vector2 m_vLinkToPos;
  EGoToLinkStatus m_eGoToLinkStatus;

  Ogre::Real m_fManaPoints;
  unsigned int m_uiKeyCount;
  unsigned int m_uiHealthPotionsCount;
  unsigned int m_uiManaPotionsCount;
  unsigned int m_uiBombCount;

#ifdef DEBUG_PLAYER_NO_COLLISION
  bool m_bPlayerNoCollisionActivated;
#endif // DEBUG_PLAYER_NO_COLLISION
public:
  CPlayer(CMap *pMap, Ogre2dManager *pSpriteManager);
  ~CPlayer();

  void startup(const Ogre::Vector2 &vPosition, Ogre::Real fDirection);

  virtual void update(Ogre::Real tpf);
  virtual void render(Ogre::Real tpf);

  void pickobject(unsigned int uiObjectId);

  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  virtual void fadeInCallback();
  virtual void fadeOutCallback();

  virtual void playerInputPauseChanged(bool bPause);
  virtual void mapRenderPauseChanged(bool bPause) {m_Fader.setVisible(!bPause);}

  Ogre::Real getManapoints() const {return m_fManaPoints;}
  void setManapoints(Ogre::Real fManapoints) {m_fManaPoints = fManapoints;}

private:
  void setupAnimations();

protected:
  virtual void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText);
  virtual void damageTakenCallback();
  virtual void killedByDamageCallback();
};

#endif
