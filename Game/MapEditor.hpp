#ifndef _MAP_EDITOR_HPP_
#define _MAP_EDITOR_HPP_

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "InputListener.hpp"
#include "PauseCaller.hpp"

class CMap;

class CMapEditor 
  : public Ogre::Singleton<CMapEditor>,
    public CInputListener,
    public CPauseCaller {
private:
  bool m_bPressed;
  bool m_bVisible;
  CEGUI::Window *m_pRoot;
  CEGUI::TabControl *m_pTabControl;
  CEGUI::Window *m_pTilesContainer;
  unsigned int m_uiCurrentTile;
  CMap *m_pMap;
public:
  static CMapEditor &getSingleton();
  static CMapEditor *getSingletonPtr();

  CMapEditor(CEGUI::Window *pRoot);


  void toggle();
  void init(CMap *pMap);
  void start();
  void stop();
  void exit();

  CEGUI::Window *getTileFromType(unsigned int uiTile);
  unsigned int getTypeFromTile(CEGUI::Window *pTile);

  virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mouseMoved( const OIS::MouseEvent &arg );
  virtual bool keyPressed( const OIS::KeyEvent &arg );
private:
  void selectTile(unsigned int uiTile);
  bool onTileClicked(const CEGUI::EventArgs &args);

  bool dummyReturnFalse(const CEGUI::EventArgs &args) {return false;}
};

#endif
