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

#ifndef _EDIT_BOX_TILE_TYPE_
#define _EDIT_BOX_TILE_TYPE_

#include "EditBoxBase.hpp"
#include "../TileTypes.hpp"

class CTile;;

class CEditBoxTileType : public CEditBoxBase {
private:
  const TileType m_ttBackup;
  TileType &m_tt;
  TileType m_uiCurrentTile;
  CEGUI::ScrollablePane *m_pTilesContainer;
  CTile *m_pTile;
public:
  CEditBoxTileType(int iID,
		   CEGUI::Window *pParent,
		   float fButtonSize,
		   const CEGUI::String &sTitle,
		   TileType &tt,
		   bool bEnableNoTile = true,
		   CTile *pTile = NULL) 
    : CEditBoxBase(iID, pParent, fButtonSize, sTitle),
      m_ttBackup(tt),
      m_tt(tt),
      m_uiCurrentTile(tt),
      m_pTile(pTile) {

    using namespace CEGUI;
    ScrollablePane *pTilesScrollBar = dynamic_cast<ScrollablePane*>(m_pWindow->createChild("OgreTray/ScrollablePane", "ScrollPane"));
    pTilesScrollBar->setDragIgnoreZone(fButtonSize * 0.2);
    m_pTilesContainer = pTilesScrollBar;
    pTilesScrollBar->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pTilesScrollBar->setSize(USize(UDim(1, 0), UDim(1, -fButtonSize)));

    int iTilesCountX = m_pWindow->getPixelSize().d_width / fButtonSize;
    int id = 0;
    if (!bEnableNoTile) {id = 1;}
    for (int y = 0; y < TT_COUNT / iTilesCountX + 1; y++) {
      for (int x = 0; x < iTilesCountX; x++){

	if (id == TT_COUNT) {
	  break;
	}

	String tileName = "Tile" + PropertyHelper<int>::toString(id) + ".png";;
      Window *pTile = pTilesScrollBar->createChild("OgreTray/StaticImage", tileName);
      //pTile->setProperty("BackgroundEnabled", "False");
      pTile->setProperty("FrameEnabled", "False");
      pTile->setProperty("Image", tileName);
      pTile->setPosition(UVector2(UDim(0, x * fButtonSize * 1.05f), UDim(0, y * fButtonSize * 1.05f)));
      pTile->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
      pTile->
	subscribeEvent(Window::EventMouseButtonUp,
		       Event::Subscriber(&CEditBoxTileType::onTileClicked, this));


      id++;
    }
    if (id == TT_COUNT) {
      break;
    }
  }
  selectTile(tt);
  }
  CEGUI::Window *getTileFromType(unsigned int uiTile) {
    using namespace CEGUI;
    String tileName = "Tile" + PropertyHelper<int>::toString(uiTile) + ".png";
    return m_pTilesContainer->getChild(tileName);
  }
  unsigned int getTypeFromTile(CEGUI::Window *pTile) {
    using namespace CEGUI;
    return PropertyHelper<int>::fromString(pTile->getName().substr(4, pTile->getName().size() - 8));
  }
  void selectTile(unsigned int uiTile) {
    using namespace CEGUI;
    if (m_uiCurrentTile != TT_COUNT) {
      getTileFromType(m_uiCurrentTile)->setProperty("FrameEnabled", "False");
    }
    if (uiTile != TT_COUNT) {
      getTileFromType(uiTile)->setProperty("FrameEnabled", "True");
    }
    m_uiCurrentTile = uiTile;
    m_tt = uiTile;
  }
  bool onTileClicked(const CEGUI::EventArgs &args) {
    using namespace CEGUI;
    if (m_pTilesContainer->wasDragged()) {return true;}
    const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
    selectTile(getTypeFromTile(wndArgs.window));
    
    return true;
  }
  CTile *getTile() const {return m_pTile;}
protected:
  bool accepted() {
    m_tt = m_uiCurrentTile;
    return true;
  }
  bool canceled() {
    m_tt = m_ttBackup;
    return true;
  }
};

#endif
