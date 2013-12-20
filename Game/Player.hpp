#ifndef PLAYER_H
#define PLAYER_H

#include "AnimatedSprite.hpp"
#include "InputListener.hpp"

class CMap;

class CPlayer : public CAnimatedSprite, public CInputListener {
private:
  enum ELastDirection {
    LD_LEFT,
    LD_RIGHT
  };
private:
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
    ANIM_THROW_RIGHT,
    ANIM_THROW_LEFT,
    ANIM_COLUMN_RIGHT,
    ANIM_COLUMN_LEFT,

    ANIM_COUNT
  };
private:
  CMap *m_pMap;
  bool m_bRightPressed;
  bool m_bLeftPressed;
  bool m_bAttackPressed;
  bool m_bJumpPressed;
  Ogre::Real m_fMaxWalkSpeed;
  Ogre::Real m_fInitialJumpSpeed;
  Ogre::Vector2 m_vCurrentSpeed;

  bool m_bOnGround;
  bool m_bJumping;
  ELastDirection m_eLastDirection;
public:
  CPlayer(CMap *pMap, Ogre2dManager *pSpriteManager);
  ~CPlayer();

  virtual void update(Ogre::Real tpf);

  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

private:
  void setupAnimations();

protected:
  virtual void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText);
};

#endif
