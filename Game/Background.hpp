#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Sprite.hpp"

class CBackground : public CSprite {
private:
  Ogre::Vector2 m_vCamerPosition;
public:
  virtual void render(Ogre::Real tpf);
};

#endif // BACKGROUND_HPP
