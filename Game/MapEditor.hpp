#ifndef _MAP_EDITOR_HPP_
#define _MAP_EDITOR_HPP_

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "InputListener.hpp"
#include "PauseCaller.hpp"
#include "MapInfo.hpp"

class CSprite;
class CMap;

class CMapEditor 
  : public Ogre::Singleton<CMapEditor>,
    public CInputListener,
    public CPauseCaller {
public:
  enum EBrushes {
    B_PLACE,
    B_EDIT,
    B_MOVE,

    B_COUNT,
  };
private:
  bool m_bPressed;
  bool m_bVisible;
  CEGUI::Window *m_pRoot;
  CEGUI::TabControl *m_pTabControl;
  CEGUI::ScrollablePane *m_pTilesContainer;
  unsigned int m_uiCurrentTile;
  CMap *m_pMap;
  EBrushes m_eSelectedBrush;
  CSprite *m_pSelectedSprite;
  bool m_bSnapToGrid;
  CMapInfoPtr m_pMapInfo;
public:
  static CMapEditor &getSingleton();
  static CMapEditor *getSingletonPtr();

  CMapEditor(CEGUI::Window *pRoot);


  void toggle();
  void init(CMap *pMap, const CMapInfoConstPtr pMapInfo);
  void start();
  void stop();
  void exit();
  void render();

  CEGUI::Window *getTileFromType(unsigned int uiTile);
  unsigned int getTypeFromTile(CEGUI::Window *pTile);

  virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mouseMoved( const OIS::MouseEvent &arg );
  virtual bool keyPressed( const OIS::KeyEvent &arg );
private:
  void handleBrushPressed(const Ogre::Vector2 &vPos);
  void handleBrushReleased(const Ogre::Vector2 &vPos);
  void handleBrushMoved(const Ogre::Vector2 &vPos, const Ogre::Vector2 &vDelta);
  void placeCurrentTile(const Ogre::Vector2 &vPos);
  void selectTile(unsigned int uiTile);
  bool onTileClicked(const CEGUI::EventArgs &args);
  bool onBrushSelectionChanged(const CEGUI::EventArgs &args);
  bool onSnapToGridChanged(const CEGUI::EventArgs &args);
  bool onSaveMap(const CEGUI::EventArgs &args);

  bool dummyReturnFalse(const CEGUI::EventArgs &args) {return false;}

  bool selectSprite(const Ogre::Vector2 &vPos);
};

#endif
