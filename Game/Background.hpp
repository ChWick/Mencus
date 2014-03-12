#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Sprite.hpp"

extern const Ogre::Vector2 TILES_PER_SCREEN;
extern const Ogre::Real SCREEN_RATIO;

class CBackground : public CSprite {
public:
  enum ETypes {
    BT_CLOUDS,
    BT_FOREST,
    BT_STARS,
    BT_FOG,

    BT_COUNT
  };

  static ETypes fromString(const Ogre::String &sName);
private:
  const ETypes m_eBackgroundType;
  const Ogre::Vector2 &m_vCameraPosition;
  const Ogre::String m_sName;
public:
  CBackground(Ogre2dManager *pSpriteManager, const Ogre::Vector2 &vCameraPosition, const Ogre::String &sName);

  const Ogre::String &getName() const {return m_sName;}

  virtual void render(Ogre::Real tpf);
};

#endif // BACKGROUND_HPP
