#ifndef SPRITE_H
#define SPRITE_H

#include <Ogre.h>
#include "ogre2d-main.hpp"
#include <vector>
#include <string>
#include "BoundingBox2d.hpp"
#include "SpriteTexture.hpp"

using namespace std;

class CSpriteTransformPipeline;


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
  void setCenter(const Ogre::Vector2 &vCenter) {m_vPosition = vCenter - m_vSize / 2;}

  const CBoundingBox2d &getBoundingBox() const {return m_bbRelativeBoundingBox;}
  CBoundingBox2d getWorldBoundingBox() const {return m_bbRelativeBoundingBox.translate(m_vPosition);}
};

#endif
