#include "Enemy.hpp"
#include "Map.hpp"
#include "Util.hpp"

const Ogre::Vector2 ENEMY_SIZE[CEnemy::ET_COUNT] = {
  Ogre::Vector2(1, 1)
};
const Ogre::Real ENEMY_SPEED[CEnemy::ET_COUNT] = {
  1.0
};

CEnemy::CEnemy(CMap &map,
	       const Ogre::Vector2 &vPosition,
	       EEnemyTypes eEnemyType,
	       Ogre::Real fDirection)
  : CAnimatedSprite(&map, map.get2dManager(), vPosition, ENEMY_SIZE[eEnemyType]),
    m_Map(map),
    m_eEnemyType(eEnemyType),
    m_fSpeed(fDirection * ENEMY_SPEED[eEnemyType]) {
  setup();
}
void CEnemy::update(Ogre::Real tpf) {
  m_vPosition.x += m_fSpeed * tpf;
  CAnimatedSprite::update(tpf);
}
void CEnemy::setup() {
  switch (m_eEnemyType) {
  case ET_GREEN_MONSTER:
    init(ENEMY_SPEED[m_eEnemyType], 2);
    setDefaultGetPath(&getEnemyTexturePath<1>);
    setupAnimation(AS_WALK_LEFT, "walk_right", 4, CSpriteTexture::MIRROR_Y);
    setupAnimation(AS_WALK_RIGHT, "walk_right", 4);
    break;
  default:
    throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, "Enemy type '" + Ogre::StringConverter::toString(m_eEnemyType) + "' is unknown", __FILE__);
  }
}
