#include "Sprite.hpp"
#include "SpriteTransformPipeline.hpp"

const Ogre::Vector2 CSpriteTexture::DEFAULT_TEXTURE_TOP_LEFT(0, 1);
const Ogre::Vector2 CSpriteTexture::DEFAULT_TEXTURE_BOTTOM_RIGHT(1, 0);

CSprite::CSprite(const CSpriteTransformPipeline *pTransformPipeline, Ogre2dManager *pSpriteManager, const Ogre::Vector2 &vPosition, const Ogre::Vector2 &vSize, const Ogre::Vector2 &vScale, const Ogre::Radian radRotation)
  : m_pTransformPipeline(pTransformPipeline),
    m_pSpriteManager(pSpriteManager),
    m_vPosition(vPosition),
    m_vSize(vSize),
    m_vScale(vScale),
    m_radRotation(radRotation),
    m_pTextureToDraw(&m_Texture),
    m_bbRelativeBoundingBox(Ogre::Vector2::ZERO, vSize * vScale),
    m_Colour(Ogre::ColourValue::White) {
}
CSprite::CSprite(const CSprite &src)
  : m_pSpriteManager(src.m_pSpriteManager),
    m_vPosition(src.m_vPosition),
    m_vSize(src.m_vSize),
    m_vScale(src.m_vScale),
    m_radRotation(src.m_radRotation),
    m_Texture(src.m_Texture),
    m_pTextureToDraw(src.m_pTextureToDraw),
    m_bbRelativeBoundingBox(src.m_bbRelativeBoundingBox),
    m_Colour(src.m_Colour) {
}
CSprite::~CSprite() {
}
void CSprite::update(Ogre::Real tpf) {
}
void CSprite::render(Ogre::Real tpf) {
  //Ogre::LogManager::getSingleton().logMessage("Test1" + Ogre::StringConverter::toString(m_vPosition) + " //// " + Ogre::StringConverter::toString(m_vSize));
  Ogre::Vector2 vStart = m_pTransformPipeline->transformPosition(m_vPosition + m_pTextureToDraw->getSpriteOffset());
  Ogre::Vector2 vEnd = m_pTransformPipeline->transformPosition(m_vPosition + m_pTextureToDraw->getSpriteOffset() + m_vSize * m_pTextureToDraw->getSpriteScale());

  if (vEnd.x < -1 || vEnd.y < -1 || vStart.x > 1 || vStart.y > 1) {
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

}
void CSprite::setTexture(const string &sName) {
  m_Texture.setTexture(Ogre::TextureManager::getSingleton().getByName(sName));
  if (m_Texture.getTexture().isNull()) {
    m_Texture.setTexture(Ogre::TextureManager::getSingleton().load(sName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME));
    //throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, sName + " is not found as a texture!", __FILE__);
  }
}
