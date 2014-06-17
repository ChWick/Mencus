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

#include "Sprite.hpp"
#include "SpriteTransformPipeline.hpp"
#include "XMLHelper.hpp"
#include "Map.hpp"

using namespace XMLHelper;


const Ogre::Vector2 CSpriteTexture::DEFAULT_TEXTURE_TOP_LEFT(0, 1);
const Ogre::Vector2 CSpriteTexture::DEFAULT_TEXTURE_BOTTOM_RIGHT(1, 0);

CSprite::CSprite(CMap &map,
		 const std::string &sID,
		 CEntity *pParent,
		 const CSpriteTransformPipeline *pTransformPipeline,
		 Ogre2dManager *pSpriteManager,
		 const Ogre::Vector2 &vPosition,
		 const Ogre::Vector2 &vSize,
		 const Ogre::Vector2 &vScale,
		 const Ogre::Radian radRotation)
  : CPhysicsEntity(map, sID, pParent),
    m_pTransformPipeline(pTransformPipeline),
    m_pSpriteManager(pSpriteManager),
    m_radRotation(radRotation),
    m_pTextureToDraw(&m_Texture),
    m_Colour(Ogre::ColourValue::White) {

  setPosition(vPosition);
  setSize(vSize);
  setScale(vScale);
  setRelativeBoundingBox(CBoundingBox2d(Ogre::Vector2::ZERO, vSize * vScale));
}
CSprite::CSprite(CMap &map,
		 CEntity *pParent,
		 const CSpriteTransformPipeline *pTransformPipeline,
		 Ogre2dManager *pSpriteManager,
		 const tinyxml2::XMLElement *pElem,
		 const Ogre::Vector2 &vSize)
  : CPhysicsEntity(map, pParent, pElem, Ogre::Vector2::ZERO, vSize),
    m_pTransformPipeline(pTransformPipeline),
    m_pSpriteManager(pSpriteManager),
    m_radRotation(RealAttribute(pElem, "sp_radRotation", 0)),
    m_pTextureToDraw(&m_Texture),
    m_Colour(Ogre::StringConverter::parseColourValue(Attribute(pElem, "sp_colour", "1 1 1 1"), Ogre::ColourValue::White)) {
}
CSprite::CSprite(const CSprite &src)
  : CPhysicsEntity(src), 
    m_pSpriteManager(src.m_pSpriteManager),
    m_radRotation(src.m_radRotation),
    m_Texture(src.m_Texture),
    m_pTextureToDraw(src.m_pTextureToDraw),
    m_Colour(src.m_Colour) {

  setPosition(src.m_vPosition);
  setSize(src.m_vSize);
  setScale(src.m_vScale);
  setRelativeBoundingBox(src.m_bbRelativeBoundingBox);
}
CSprite::~CSprite() {
}
void CSprite::update(Ogre::Real tpf) {
  CPhysicsEntity::update(tpf);
}
void CSprite::render(Ogre::Real tpf) {
  if (!m_bVisible) {return;}
  //Ogre::LogManager::getSingleton().logMessage("Test1" + Ogre::StringConverter::toString(m_vPosition) + " //// " + Ogre::StringConverter::toString(m_vSize));
  Ogre::Vector2 vStart = m_pTransformPipeline->transformPosition(m_vPosition + m_pTextureToDraw->getSpriteOffset());
  Ogre::Vector2 vEnd = m_pTransformPipeline->transformPosition(m_vPosition + m_pTextureToDraw->getSpriteOffset() + m_vSize * m_pTextureToDraw->getSpriteScale());

  if (!m_pTransformPipeline->isVisible(vStart, vEnd)) {
    // not visible
    return;
  }

  m_pSpriteManager->spriteBltFull(
				  m_pTextureToDraw->getTexture()->getName(),
				  vStart.x,
				  vStart.y,
				  vEnd.x,
				  vEnd.y,
				  m_pTextureToDraw->getTexturePosTopLeft().x,
				  m_pTextureToDraw->getTexturePosTopLeft().y,
				  m_pTextureToDraw->getTexturePosBottomRight().x,
				  m_pTextureToDraw->getTexturePosBottomRight().y,
          m_Colour);

  CPhysicsEntity::render(tpf);
}
void CSprite::setTexture(const string &sName) {
  m_Texture.setTexture(Ogre::TextureManager::getSingleton().getByName(sName));
  if (m_Texture.getTexture().isNull()) {
    m_Texture.setTexture(Ogre::TextureManager::getSingleton().load(sName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
    //throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, sName + " is not found as a texture!", __FILE__);
  }
}
void CSprite::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CPhysicsEntity::writeToXMLElement(pElem, eStyle);
  
  using namespace XMLHelper;

  if (eStyle == OS_FULL) {
    SetAttribute(pElem, "sp_radRotation", m_radRotation.valueRadians());
    SetAttribute<Ogre::ColourValue>(pElem, "sp_colour", m_Colour);
  }
}
