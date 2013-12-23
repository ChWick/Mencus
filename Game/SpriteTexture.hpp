#ifndef SPRITE_TEXTURE_HPP
#define SPRITE_TEXTURE_HPP

#include <Ogre.h>

class CSpriteTexture {
public:
  enum EMirrorTypes {
    MIRROR_NONE,
    MIRROR_X,
    MIRROR_Y,
    MIRROR_XY,
  };

  static const Ogre::Vector2 DEFAULT_TEXTURE_TOP_LEFT;
  static const Ogre::Vector2 DEFAULT_TEXTURE_BOTTOM_RIGHT;
private:
  Ogre::Vector2 m_vTexturePosTopLeft;
  Ogre::Vector2 m_vTexturePosBottomRight;
  Ogre::Vector2 m_vSpriteOffset;
  Ogre::Vector2 m_vSpriteScale;
  Ogre::TexturePtr m_pTexture;
public:
  CSpriteTexture(Ogre::TexturePtr pTexture = Ogre::TexturePtr(),
		 const Ogre::Vector2 &vTexturePosTopLeft = DEFAULT_TEXTURE_TOP_LEFT,
		 const Ogre::Vector2 &vTexturePosBottomRight = DEFAULT_TEXTURE_BOTTOM_RIGHT,
		 const Ogre::Vector2 &vSpriteOffset = Ogre::Vector2::ZERO,
		 const Ogre::Vector2 &vSpriteScale = Ogre::Vector2::UNIT_SCALE)
    :
    m_vTexturePosTopLeft(vTexturePosTopLeft),
    m_vTexturePosBottomRight(vTexturePosBottomRight),
    m_pTexture(pTexture),
    m_vSpriteOffset(vSpriteOffset),
    m_vSpriteScale(vSpriteScale) {
  }
  CSpriteTexture(const CSpriteTexture &src)
  :
  m_vTexturePosTopLeft(src.m_vTexturePosTopLeft),
  m_vTexturePosBottomRight(src.m_vTexturePosBottomRight),
  m_pTexture(src.m_pTexture),
  m_vSpriteOffset(src.m_vSpriteOffset),
  m_vSpriteScale(src.m_vSpriteScale) {
  }
  
  const CSpriteTexture &operator=(const CSpriteTexture &src) {
    m_vTexturePosTopLeft = src.m_vTexturePosTopLeft;
    m_vTexturePosBottomRight = src.m_vTexturePosBottomRight;
    m_pTexture = src.m_pTexture;
    m_vSpriteOffset = src.m_vSpriteOffset;
    m_vSpriteScale = src.m_vSpriteScale;

    return *this;
  }

  const Ogre::Vector2 &getTexturePosTopLeft() const {return m_vTexturePosTopLeft;}
  const Ogre::Vector2 &getTexturePosBottomRight() const {return m_vTexturePosBottomRight;}
  Ogre::TexturePtr getTexture() const {return m_pTexture;}
  const Ogre::Vector2 &getSpriteOffset() const {return m_vSpriteOffset;}
  const Ogre::Vector2 &getSpriteScale() const {return m_vSpriteScale;}

  void setTexturePosTopLeft(const Ogre::Vector2 &vPos) {m_vTexturePosTopLeft = vPos;}
  void setTexturePosBottomRight(const Ogre::Vector2 &vPosition) {m_vTexturePosBottomRight = vPosition;}

  void setTexturePosRight(const Ogre::Real x) {m_vTexturePosBottomRight.x = x;} 
  void setTexture(Ogre::TexturePtr pTexture) {m_pTexture = pTexture;}
  void setSpriteOffset(const Ogre::Vector2 &vOffset) {m_vSpriteOffset = vOffset;}
  void setSpriteScale(const Ogre::Vector2 &vScale) {m_vSpriteScale = vScale;}
  
  void mirror(EMirrorTypes mt) {
    switch (mt) {
    case MIRROR_Y:
      swap(m_vTexturePosTopLeft.x, m_vTexturePosBottomRight.x);
      break;
    case MIRROR_X:
      swap(m_vTexturePosTopLeft.y, m_vTexturePosBottomRight.y);
      break;
    case MIRROR_XY:
      swap(m_vTexturePosTopLeft.x, m_vTexturePosBottomRight.x);
      swap(m_vTexturePosTopLeft.y, m_vTexturePosBottomRight.y);
      break;
    }
  }
};

#endif
