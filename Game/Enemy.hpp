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
private:
  const EEnemyTypes m_eEnemyType;
  CMap &m_Map;
public:
  CEnemy(CMap &map, const Ogre::Vector2 &vPosition, EEnemyTypes eEnemyType);

  void update(Ogre::Real tpf);
};

#endif
