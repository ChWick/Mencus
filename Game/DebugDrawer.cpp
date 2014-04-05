#include "DebugDrawer.hpp"
#include "Tile.hpp"
#include "BoundingBox2d.hpp"
#include "Util.hpp"
#include "Link.hpp"
#include "Switch.hpp"
#include "Map.hpp"
#include "ChangeTileEvent.hpp"

template<> CDebugDrawer *Ogre::Singleton<CDebugDrawer>::msSingleton = 0;

CDebugDrawer *CDebugDrawer::getSingletonPtr() {
  return msSingleton;
}
CDebugDrawer &CDebugDrawer::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}
CDebugDrawer::CDebugDrawer(CMap &map) {
  m_DebugTextures[DT_PINK] = getTextureInGfx("debug_pink.png");
  m_DebugTextures[DT_BLUE] = getTextureInGfx("debug_blue.png");
  m_DebugTextures[DT_GREEN] = getTextureInGfx("debug_green.png");

  m_pDebugSprite = new CSprite(map,
			       "DebugTile",
			       NULL,
			       &map,
			       map.getDebug2dManager(),
			       Ogre::Vector2::ZERO,
			       Ogre::Vector2::UNIT_SCALE);
}
CDebugDrawer::~CDebugDrawer() {
  delete m_pDebugSprite;
  m_pDebugSprite = NULL;
}
void CDebugDrawer::draw(const CTile *pTile, unsigned int uiTT, float fAlpha) {
  m_pDebugSprite->setTexture(getTileTexturePath(uiTT));
  m_pDebugSprite->setPosition(pTile->getPosition());
  m_pDebugSprite->setSize(pTile->getSize());
  m_pDebugSprite->setAlpha(fAlpha);
  m_pDebugSprite->render(0);
  m_pDebugSprite->setAlpha(1);
}
void CDebugDrawer::draw(const CTile *pTile, EDebugTextures dt) const {
  m_pDebugSprite->setTexture(m_DebugTextures[dt]);
  m_pDebugSprite->setPosition(pTile->getPosition());
  m_pDebugSprite->setSize(pTile->getSize());
  m_pDebugSprite->render(0);
}
void CDebugDrawer::draw(const CBoundingBox2d &bb) const {
  m_pDebugSprite->setTexture(m_DebugTextures[DT_PINK]);
  m_pDebugSprite->setPosition(bb.getPosition());
  m_pDebugSprite->setSize(bb.getSize());
  m_pDebugSprite->render(0);
}
void CDebugDrawer::draw(const CLink &link) const {
  m_pDebugSprite->setTexture(m_DebugTextures[DT_GREEN]);
  m_pDebugSprite->setSize(CTile::DEFAULT_TILE_SIZE);
  m_pDebugSprite->setPosition(Ogre::Vector2(link.getFirstX(), link.getFirstY()));
  m_pDebugSprite->render(0);
  m_pDebugSprite->setPosition(Ogre::Vector2(link.getSecondX(), link.getSecondY()));
  m_pDebugSprite->render(0);
}
void CDebugDrawer::draw(const CEvent *pEvent, float fAlpha) const {
  switch (pEvent->getType()) {
  case CEvent::EVENT_CHANGE_TILE:
    {
      const CChangeTileEvent *pCCEvent(dynamic_cast<const CChangeTileEvent *>(pEvent));
      m_pDebugSprite->setTexture(getTileTexturePath(pCCEvent->getTileType()));
      m_pDebugSprite->setSize(CTile::DEFAULT_TILE_SIZE);
      m_pDebugSprite->setPosition(Ogre::Vector2(pCCEvent->getTilePosX(), pCCEvent->getTilePosY()));
      m_pDebugSprite->setAlpha(fAlpha);
      m_pDebugSprite->render(0);
      m_pDebugSprite->setAlpha(1);
    }
    break;
  default:
    break;
  }
}
