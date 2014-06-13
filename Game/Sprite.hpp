/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef SPRITE_H
#define SPRITE_H

#include <Ogre.h>
#include "PhysicsEntity.hpp"
#include "ogre2d-main.hpp"
#include <vector>
#include "SpriteTexture.hpp"
#include <tinyxml2.h>

using namespace std;

class CSpriteTransformPipeline;
class CMap;


class CSprite : public CPhysicsEntity {
protected:
  const CSpriteTransformPipeline *m_pTransformPipeline; //!<
  Ogre2dManager *m_pSpriteManager;			//
  Ogre::Radian m_radRotation;				//
  CSpriteTexture m_Texture;				//!< Texture of the sprite
  const CSpriteTexture *m_pTextureToDraw;		//!< Texture to draw in the update method (this is modified e.g. bywriteToXMLElement(tinyxml2::XMLElement *pElem the animated sprite)
  Ogre::ColourValue m_Colour;               //!< The draw colour
public:
  CSprite(CMap &map,
	  const std::string &sID,
	  CEntity *pParent,
	  const CSpriteTransformPipeline *pTransformPipeline,
	  Ogre2dManager *pSpriteManager,
	  const Ogre::Vector2 &vPosition,
	  const Ogre::Vector2 &vSize,
	  const Ogre::Vector2 &vScale = Ogre::Vector2::UNIT_SCALE,
	  const Ogre::Radian radRotation = Ogre::Radian(0));

  CSprite(CMap &map,
	  CEntity *pParent,
	  const CSpriteTransformPipeline *pTransformPipeline,
	  Ogre2dManager *pSpriteManager,
	  const tinyxml2::XMLElement *pElem,
	  const Ogre::Vector2 &vSize = Ogre::Vector2::UNIT_SCALE);

  CSprite(const CSprite &src);
  virtual ~CSprite();

  void setTexture(const CSpriteTexture &texture) {m_Texture = texture;}
  void setTexture(Ogre::TexturePtr pTexture) {m_Texture.setTexture(pTexture);}
  void setTexture(const string &sName);
  void setTextureToDraw(const CSpriteTexture *pTexture) {m_pTextureToDraw = pTexture;}
  CSpriteTexture &getTexture() {return m_Texture;}

  void setTextureCoords(const Ogre::Vector2 &vTopLeft, const Ogre::Vector2 &vSize) {m_Texture.setTexturePosTopLeft(vTopLeft); m_Texture.setTexturePosBottomRight(vSize);}

  virtual void update(Ogre::Real tpf);
  virtual void render(Ogre::Real tpf);

  void setColour(const Ogre::ColourValue &colour) {m_Colour = colour;}
  const Ogre::ColourValue &getColour() const {return m_Colour;}

  void setAlpha(Ogre::Real fAlpha) {m_Colour.a = fAlpha;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;
};

#endif
