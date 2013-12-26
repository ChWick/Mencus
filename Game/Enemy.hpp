#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "AnimatedSprite.hpp"
#include "HitableObject.hpp"

class CMap;

class CEnemy : public CAnimatedSprite, public CHitableObject {
public:
  enum EEnemyTypes {
    ET_GREEN_MONSTER,
    ET_COUNT,
  };
  enum EAttackSequences {
    AS_WALK_LEFT,
    AS_WALK_RIGHT,
    AS_ATTACK_LEFT,
    AS_ATTACK_RIGHT,
    AS_JUMP_LEFT,
    AS_JUMP_RIGHT,
  };
private:
  const EEnemyTypes m_eEnemyType;
  CMap &m_Map;
  Ogre::Vector2 m_vSpeed;
  bool m_bOnGround;
public:
  CEnemy(CMap &map,
    const Ogre::Vector2 &vPosition,
    EEnemyTypes eEnemyType,
    Ogre::Real fDirection,
    Ogre::Real fHitpoints);

  void update(Ogre::Real tpf);

protected:
  void killedByDamageCallback();
  void animationTextureChangedCallback(unsigned int uiOldText, unsigned uiNewText);
private:
  void setup();
  void updateKI();
};

#endif
