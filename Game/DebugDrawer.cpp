#include "DebugDrawer.hpp"
#include "Tile.hpp"
#include "BoundingBox2d.hpp"
#include "Util.hpp"

template<> CDebugDrawer *Ogre::Singleton<CDebugDrawer>::msSingleton = 0;

CDebugDrawer *CDebugDrawer::getSingletonPtr() {
  return msSingleton;
}
CDebugDrawer &CDebugDrawer::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}
CDebugDrawer::CDebugDrawer(const CSpriteTransformPipeline *pTransformPipeline,
			   Ogre2dManager *pSpriteManager) {
  m_DebugTextures[DT_PINK] = getTextureInGfx("debug_pink.png");

  m_pDebugSprite = new CSprite(pTransformPipeline,
			       pSpriteManager,
			       Ogre::Vector2::ZERO,
			       Ogre::Vector2::UNIT_SCALE);
}
CDebugDrawer::~CDebugDrawer() {
  delete m_pDebugSprite;
  m_pDebugSprite = NULL;
}
void CDebugDrawer::draw(const CTile *pTile) const {
  m_pDebugSprite->setTexture(m_DebugTextures[DT_PINK]);
  m_pDebugSprite->setPosition(pTile->getPosition());
  m_pDebugSprite->setSize(pTile->getSize());
  m_pDebugSprite->draw();
}
void CDebugDrawer::draw(const CBoundingBox2d &bb) const {
  m_pDebugSprite->setTexture(m_DebugTextures[DT_PINK]);
  m_pDebugSprite->setPosition(bb.getPosition());
  m_pDebugSprite->setSize(bb.getSize());
  m_pDebugSprite->draw();
}
