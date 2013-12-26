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
    ET_KNIGHT,
    ET_BEAR,

    ET_COUNT,
  };
private:
  CMap *m_pMap;
  const EExplosionTypes m_eExplosionType;
  Ogre::Real m_fFadingTimer;                 //!< after the last image the fading timer (alpha blending)
public:
  CExplosion(CMap *pMap, const Ogre::Vector2 &vCenter, EExplosionTypes eExplosionType);

  void update(Ogre::Real tpf);

  void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText);
};

#endif
