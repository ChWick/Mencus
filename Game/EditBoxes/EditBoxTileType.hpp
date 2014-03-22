#ifndef _EDIT_BOX_TILE_TYPE_
#define _EDIT_BOX_TILE_TYPE_

#include "EditBoxBase.hpp"
#include "../TileTypes.hpp"


class CEditBoxTileType : public CEditBoxBase {
private:
  TileType &m_tt;
public:
  CEditBoxTileType(CEGUI::Window *pParent,
		float fButtonSize,
		const CEGUI::String &sTitle,
		TileType &tt) 
    : CEditBoxBase(pParent, fButtonSize, sTitle),
      m_tt(tt) {

    using namespace CEGUI;
  }
protected:
  bool accepted() {

    return true;
  }
};

#endif
