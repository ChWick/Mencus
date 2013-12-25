#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "AnimatedSprite.hpp"

class CMap;

class CEnemy : public CAnimatedSprite {
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
  };
private:
  const EEnemyTypes m_eEnemyType;
  CMap &m_Map;
  Ogre::Real m_fSpeed;
public:
  CEnemy(CMap &map,
	 const Ogre::Vector2 &vPosition,
	 EEnemyTypes eEnemyType,
	 Ogre::Real fDirection);

  void update(Ogre::Real tpf);

private:
  void setup();
};

#endif
