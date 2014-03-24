#include "Tile.hpp"
#include "ogre2d-main.hpp"
#include "DebugDrawer.hpp"

CTile::CTile(CSpriteTransformPipeline *pTransformPipeline, Ogre2dManager* p2dManagerMap, const Ogre::Vector2 &vPosition, TileType ttTileType)
  : CSprite(pTransformPipeline, p2dManagerMap, vPosition, DEFAULT_TILE_SIZE),
    m_uiMapPosX(static_cast<unsigned int>(vPosition.x + 0.5)),
    m_uiMapPosY(static_cast<unsigned int>(vPosition.y + 0.5)),
    m_uiTileFlags(getTileFlags(ttTileType)),
    m_ttTileType(ttTileType),
    m_ttEndangeredTileType(TT_NONE) {
  setTexture(DEFAULT_TILE_TEXTURE_NAME + Ogre::StringConverter::toString(ttTileType) + ".png");
}

CTile::CTile(const CTile &src)
  : CSprite(src),
    m_uiMapPosX(src.m_uiMapPosX),
    m_uiMapPosY(src.m_uiMapPosY),
    m_uiTileFlags(src.m_uiTileFlags),
    m_ttTileType(src.m_ttTileType),
    m_ttEndangeredTileType(src.m_ttEndangeredTileType) {
}
void CTile::update(Ogre::Real tpf) {
  CSprite::update(tpf);

#ifdef DEBUG_ENDANGERED_TILES
  if ((m_uiTileFlags & TF_ENDANGERED) == TF_ENDANGERED) {
    CDebugDrawer::getSingleton().draw(this, CDebugDrawer::DT_BLUE);
  }
#endif
}
void CTile::setEndangeredTileType(TileType tt) {
  m_ttEndangeredTileType = tt;
  m_uiTileFlags |= TF_ENDANGERED;
}
unsigned int CTile::getTileFlags(TileType tt) {
  unsigned int uiFlags = 0;
  switch (tt) {
  case 1:
  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 11:
  case 12:
  case 13:
  case 14:
  case 15:
  case 16:
  case 17:
  case 18:
  case 19:
  case 20:
  case 24:
  case 31:
  case 32:
  case 33:
  case 34:
  case 35:
  case 36:
  case 37:
  case 38:
  case 39:
  case 40:
  case 41:
  case 42:
  case 43:
  case 45:
  case 46:
  case 47:
  case 48:
  case 53:
  case 71:
  case 72:
  case 73:
  case 74:
  case 75:
  case 76:
  case 77:
  case 78:
  case 79:
  case 81:
  case 82:
  case 83:
  case 92:
  case 93:
  case 94:
  case 26: uiFlags += TF_UNPASSABLE; break;
  case 49:
  case 50: uiFlags += TF_LOCK; uiFlags += TF_UNPASSABLE; break;
  case 52: uiFlags += TF_DAMAGES; uiFlags += TF_UNPASSABLE; break;
  case 21: uiFlags += TF_DOOR1; uiFlags += TF_PASSABLE; break;
  case 22: uiFlags += TF_DOOR2; uiFlags += TF_PASSABLE; break;
  case 63: uiFlags += TF_DOOR2; uiFlags += TF_PASSABLE; break;
  case 64: uiFlags += TF_DOOR3; uiFlags += TF_UNPASSABLE; break;
  case 65: uiFlags += TF_DOOR1; uiFlags += TF_PASSABLE; break;
  case 66: uiFlags += TF_DOOR2; uiFlags += TF_PASSABLE; break;
  case 67: uiFlags += TF_PASSABLE; uiFlags += TF_CHANGEBLOCK; break;
  case 68: uiFlags += TF_UNPASSABLE; uiFlags += TF_CHANGEBLOCK; break;
  case 23:
  case 25:
  case 27:
  case 28:
  case 29:
  case 30:
  case 44:
  case 51:
  case 54:
  case 55:
  case 56:
  case 57:
  case 58:
  case 59:
  case 61:
  case 62:
  case 69:
  case 70:
  case 80:
  case 84:
  case 85:
  case 86:
  case 87:
  case 88:
  case 89:
  case 90:
  case 91:
  case 95:
  case 96:
  case 97:
  case 98:
  case 99:
  case 100:
  case 101:
  case 102:
  case 2: uiFlags += TF_PASSABLE; break;
  }
  return uiFlags;
}
