#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Sprite.hpp"

extern const Ogre::Vector2 TILES_PER_SCREEN;
extern const Ogre::Real SCREEN_RATIO;

class CBackground : public CSprite {
private:
  const Ogre::Vector2 &m_vCameraPosition;
public:
  CBackground(Ogre2dManager *pSpriteManager, const Ogre::Vector2 &vCameraPosition, const Ogre::String &sTexture);

  virtual void render(Ogre::Real tpf);
};

#endif // BACKGROUND_HPP
