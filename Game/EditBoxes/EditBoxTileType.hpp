#ifndef _EDIT_BOX_TILE_TYPE_
#define _EDIT_BOX_TILE_TYPE_

#include "EditBoxBase.hpp"
#include "../TileTypes.hpp"

class CEditBoxTileType : public CEditBoxBase {
private:
  const TileType m_ttBackup;
  TileType &m_tt;
  TileType m_uiCurrentTile;
  CEGUI::ScrollablePane *m_pTilesContainer;
public:
  CEditBoxTileType(CEGUI::Window *pParent,
		float fButtonSize,
		const CEGUI::String &sTitle,
		TileType &tt) 
    : CEditBoxBase(pParent, fButtonSize, sTitle),
      m_tt(tt), m_uiCurrentTile(tt), m_ttBackup(tt) {

    using namespace CEGUI;
    ScrollablePane *pTilesScrollBar = dynamic_cast<ScrollablePane*>(m_pWindow->createChild("OgreTray/ScrollablePane", "ScrollPane"));
    pTilesScrollBar->setDragIgnoreZone(fButtonSize * 0.2);
    m_pTilesContainer = pTilesScrollBar;
    pTilesScrollBar->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pTilesScrollBar->setSize(USize(UDim(1, 0), UDim(1, -fButtonSize)));

    int iTilesCountX = m_pWindow->getPixelSize().d_width / fButtonSize;
    int id = 0;
    for (int y = 0; y < TT_COUNT / iTilesCountX + 1; y++) {
      for (int x = 0; x < iTilesCountX; x++){
	id++; // tiles count start at 1

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
