#ifndef SPRITE_H
#define SPRITE_H

#include <Ogre.h>
#include "ogre2d-main.hpp"
#include <vector>
#include <string>
#include "BoundingBox2d.hpp"

using namespace std;

class CSpriteTransformPipeline;

enum EMirrorTypes {
  MIRROR_NONE,
  MIRROR_X,
  MIRROR_Y,
  MIRROR_XY,
};

class CSpriteTexture {
public:
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

class CSprite {
protected:
  const CSpriteTransformPipeline *m_pTransformPipeline; //!< 
  Ogre2dManager *m_pSpriteManager;			// 
  Ogre::Vector2 m_vPosition;				// 
  Ogre::Vector2 m_vSize;				// 
  Ogre::Vector2 m_vScale;				// 
  Ogre::Radian m_radRotation;				// 
  CSpriteTexture m_Texture;				//!< Texture of the sprite
  const CSpriteTexture *m_pTextureToDraw;		//!< Texture to draw in the update method (this is modified e.g. by the animated sprite)
  CBoundingBox2d m_bbRelativeBoundingBox;		//!< Bounding box, realative position
public:
  CSprite(const CSpriteTransformPipeline *pTransformPipeline,
	  Ogre2dManager *pSpriteManager,
	  const Ogre::Vector2 &vPosition,
	  const Ogre::Vector2 &vSize,
	  const Ogre::Vector2 &vScale = Ogre::Vector2::UNIT_SCALE,
	  const Ogre::Radian radRotation = Ogre::Radian(0));

  CSprite(const CSprite &src);
  virtual ~CSprite();

  void setTexture(Ogre::TexturePtr pTexture) {m_Texture.setTexture(pTexture);}
  void setTexture(const string &sName);
  void setTextureToDraw(const CSpriteTexture *pTexture) {m_pTextureToDraw = pTexture;}

  void setTextureCoords(const Ogre::Vector2 &vTopLeft, const Ogre::Vector2 &vSize) {m_Texture.setTexturePosTopLeft(vTopLeft); m_Texture.setTexturePosBottomRight(vSize);}

  virtual void update(Ogre::Real tpf);
  void draw();


  const Ogre::Vector2 &getPosition() const {return m_vPosition;}
  void setPosition(const Ogre::Vector2 &vPosition) {m_vPosition = vPosition;}

  const Ogre::Vector2 &getSize() const {return m_vSize;}
  void setSize(const Ogre::Vector2 &vSize) {m_vSize = vSize;}

  Ogre::Vector2 getCenter() const {return m_vPosition + m_vSize / 2;}
  const CBoundingBox2d &getBoundingBox() const {return m_bbRelativeBoundingBox;}
  CBoundingBox2d getWorldBoundingBox() const {return m_bbRelativeBoundingBox.translate(m_vPosition);}
};

#endif
