#include "ChangeTileEvent.hpp"
#include "Map.hpp"
#include "TileTypes.hpp"
#include "Tile.hpp"

using namespace XMLHelper;

CChangeTileEvent::CChangeTileEvent(CMap &map) 
  : CEvent(map, EVENT_CHANGE_TILE),
    m_uiTileType(TT_NONE),
    m_uiOldTileType(TT_NONE),
    m_uiTilePosX(0),
    m_uiTilePosY(0) {
}
CChangeTileEvent::CChangeTileEvent(CMap &map, const tinyxml2::XMLElement *pElem)
  : CEvent(map, EVENT_CHANGE_TILE, pElem),
    m_uiTileType(IntAttribute(pElem, "id")),
    m_uiOldTileType(IntAttribute(pElem, "oldid", TT_NONE)),
    m_uiTilePosX(IntAttribute(pElem, "x")),
    m_uiTilePosY(IntAttribute(pElem, "y")) {
}
void CChangeTileEvent::init() {
  if (m_uiOldTileType == TT_NONE) {
    m_uiOldTileType = m_Map.getTile(m_uiTilePosX, m_uiTilePosY)->getTileType();
  }
}
void CChangeTileEvent::start() {
  m_Map.changeTileType(m_uiTilePosX, m_uiTilePosY, m_uiTileType);
}
void CChangeTileEvent::stop() {
  m_Map.changeTileType(m_uiTilePosX, m_uiTilePosY, m_uiOldTileType);
}
void CChangeTileEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  pElem->SetAttribute("id", m_uiTileType);
  if (eStyle == OS_FULL) {
    pElem->SetAttribute("oldid", m_uiOldTileType);
  }
  pElem->SetAttribute("x", m_uiTilePosX);
  pElem->SetAttribute("y", m_uiTilePosY);
}
