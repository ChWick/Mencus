#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Sprite.hpp"


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
  const Ogre::Vector2 &m_vTilesPerScreen;
  const float &m_fScreenRatio;
public:
  CBackground(Ogre2dManager *pSpriteManager,
	      const Ogre::Vector2 &vCameraPosition,
	      const Ogre::String &sName,
	      const Ogre::Vector2 &vTilesPerScreen,
	      const float &m_fScreenRatio);

  const Ogre::String &getName() const {return m_sName;}

  virtual void render(Ogre::Real tpf);
};

#endif // BACKGROUND_HPP
