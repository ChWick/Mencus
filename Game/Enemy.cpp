#include "Enemy.hpp"
#include "Map.hpp"

const Ogre::Vector2 ENEMY_SIZE[CEnemy::ET_COUNT] = {
  Ogre::Vector2(1, 1)
};

CEnemy::CEnemy(CMap &map, const Ogre::Vector2 &vPosition, EEnemyTypes eEnemyType) 
  : CAnimatedSprite(&map, map.get2dManager(), vPosition, ENEMY_SIZE[eEnemyType]),
    m_Map(map),
    m_eEnemyType(eEnemyType) {
}
void CEnemy::update(Ogre::Real tpf) {
}
