#ifndef PLAYER_H
#define PLAYER_H

#include "AnimatedSprite.hpp"
#include "InputListener.hpp"
#include "Fader.hpp"

class CMap;
class CShot;

class CPlayer : public CAnimatedSprite,
		public CInputListener,
		public CFaderCallback {

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

  Ogre::Vector2 m_vLinkFromPos;
  Ogre::Vector2 m_vLinkToPos;
  EGoToLinkStatus m_eGoToLinkStatus;
public:
  CPlayer(CMap *pMap, Ogre2dManager *pSpriteManager);
  ~CPlayer();

  virtual void update(Ogre::Real tpf);

  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );
  
  virtual void fadeInCallback();
  virtual void fadeOutCallback();

private:
  void setupAnimations();

protected:
  virtual void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText);
};

#endif
