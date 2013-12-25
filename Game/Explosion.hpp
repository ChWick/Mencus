#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AnimatedSprite.hpp"

class CMap;

class CExplosion : public CAnimatedSprite {
public:
  enum EExplosionTypes {
    ET_BOLT,
    ET_BOMB,
    ET_GREEN_MONSTER,

    ET_COUNT,    
  };
private:
  CMap *m_pMap;
  const EExplosionTypes m_eExplosionType;
public:
  CExplosion(CMap *pMap, const Ogre::Vector2 &vCenter, EExplosionTypes eExplosionType);

  void update(Ogre::Real tpf);

  void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText);
};

#endif
