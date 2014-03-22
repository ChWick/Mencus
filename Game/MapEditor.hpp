#ifndef _MAP_EDITOR_HPP_
#define _MAP_EDITOR_HPP_

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "InputListener.hpp"
#include "PauseCaller.hpp"
#include "PauseListener.hpp"
#include "PauseListener.hpp"
#include "MapInfo.hpp"

class CSprite;
class CMap;
class CEditBoxBase;
class SSwitchEntry;

class CMapEditor 
  : public Ogre::Singleton<CMapEditor>,
    public CInputListener,
    public CPauseCaller,
    public CPauseListener {
public:
  enum EEditButtonTypes {
    EBT_STRING,
    EBT_FLOAT,
    EBT_BOOL,
  };
  enum EEditButtons {
    EB_HITPOINTS,
    EB_DAMAGE,
    EB_JUMPING,
    EB_SWITCH_AFFECTING_BLOCKS,
  };
  enum EBrushes {
    B_PLACE,
    B_EDIT,
    B_MOVE,

    B_COUNT,
  };
private:
  float m_fButtonSize;
  bool m_bRenderPause;
  bool m_bPressed;
  bool m_bVisible;
  CEGUI::Window *m_pRoot;
  CEGUI::TabControl *m_pTabControl;
  CEGUI::ScrollablePane *m_pTilesContainer;
  CEGUI::ScrollablePane *m_pObjectsContainer;
  CEGUI::ScrollablePane *m_pEditContainer;
  CEditBoxBase *m_pEditValueWindow;
  CEGUI::Window *m_pEditSprite;
  CEGUI::Window *m_pEditSwitchPane;
  unsigned int m_uiCurrentTile;
  unsigned int m_uiCurrentObject;
  CMap *m_pMap;
  EBrushes m_eSelectedBrush;
  CSprite *m_pSelectedSprite;
  bool m_bSnapToGrid;
  CMapInfoPtr m_pMapInfo;
  bool m_bInitialized;
  
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
  
  CEGUI::Window *getObjectFromType(unsigned int uiObject);
  unsigned int getTypeFromObject(CEGUI::Window *pTile);

  virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mouseMoved( const OIS::MouseEvent &arg );

  virtual bool touchMoved(const OIS::MultiTouchEvent& evt);
  virtual bool touchPressed(const OIS::MultiTouchEvent& evt);
  virtual bool touchReleased(const OIS::MultiTouchEvent& evt);
  virtual bool touchCancelled(const OIS::MultiTouchEvent& evt);

  virtual bool keyPressed( const OIS::KeyEvent &arg );

  void resize(float fButtonSize);
  void setVisible(bool bVisible);
  void reloadTextures();
  void mapEditorUpdatePauseChanged(bool bPause);
private:
  CEGUI::ListboxTextItem *createSwitchEntry(const SSwitchEntry &entry);
  CEGUI::Window *createEditButton(CEGUI::Window *pParent,
				  EEditButtons id,
				  EEditButtonTypes type,
				  float &fCurrentHeight);
  // checks if m_bSnapping, and then snaps
  Ogre::Vector2 snappedPos(const Ogre::Vector2 &vPos);
  void handleBrushPressed(const Ogre::Vector2 &vPos);
  void handleBrushReleased(const Ogre::Vector2 &vPos);
  void handleBrushMoved(const Ogre::Vector2 &vPos, const Ogre::Vector2 &vDelta);
  void placeCurrentTile(const Ogre::Vector2 &vPos);
  void placeCurrentObject(const Ogre::Vector2 &vPos);
  void selectTile(unsigned int uiTile);
  bool onTileClicked(const CEGUI::EventArgs &args);
  void selectObject(unsigned int uiObject);
  bool onObjectClicked(const CEGUI::EventArgs &args);
  bool onBrushSelectionChanged(const CEGUI::EventArgs &args);
  bool onSnapToGridChanged(const CEGUI::EventArgs &args);
  bool onSaveMap(const CEGUI::EventArgs &args);
  bool onDelete(const CEGUI::EventArgs &args);
  bool onDeleteSwitchEntry(const CEGUI::EventArgs &args);
  bool onAddSwitchEntry(const CEGUI::EventArgs &args);

  bool onEditFloat(const CEGUI::EventArgs &args);
  bool onEditBoolChanged(const CEGUI::EventArgs &args);
  bool dummyReturnFalse(const CEGUI::EventArgs &args) {return false;}

  bool selectSprite(const Ogre::Vector2 &vPos);
  void selectedSprite(CSprite *pSprite);
};

#endif
