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

#include "ChangeTileEvent.hpp"
#include "Map.hpp"
#include "TileTypes.hpp"
#include "Tile.hpp"

using namespace XMLHelper;

CChangeTileEvent::CChangeTileEvent(CMap &map, CEntity &owner)
  : CEvent(map, owner, EVENT_CHANGE_TILE),
    m_uiTileType(TT_NONE),
    m_uiOldTileType(TT_NONE),
    m_uiTilePosX(0),
    m_uiTilePosY(0) {
}
CChangeTileEvent::CChangeTileEvent(CMap &map, CEntity &owner, const tinyxml2::XMLElement *pElem)
  : CEvent(map, owner, EVENT_CHANGE_TILE, pElem),
    m_uiTileType(IntAttribute(pElem, "tile")),
    m_uiOldTileType(IntAttribute(pElem, "old_tile", TT_NONE)),
    m_uiTilePosX(IntAttribute(pElem, "x")),
    m_uiTilePosY(IntAttribute(pElem, "y")) {
}
void CChangeTileEvent::init() {
  CEvent::init();

  if (m_uiOldTileType == TT_NONE) {
    m_uiOldTileType = m_Map.getTile(m_uiTilePosX, m_uiTilePosY)->getTileType();
  }
}
void CChangeTileEvent::start_impl() {
  m_Map.changeTileType(m_uiTilePosX, m_uiTilePosY, m_uiTileType);
}
void CChangeTileEvent::stop_impl() {
  m_Map.changeTileType(m_uiTilePosX, m_uiTilePosY, m_uiOldTileType);
}
void CChangeTileEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  pElem->SetAttribute("tile", m_uiTileType);
  if (eStyle == OS_FULL) {
    pElem->SetAttribute("old_tile", m_uiOldTileType);
  }
  pElem->SetAttribute("x", m_uiTilePosX);
  pElem->SetAttribute("y", m_uiTilePosY);
}
