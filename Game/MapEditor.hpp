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

#ifndef _MAP_EDITOR_HPP_
#define _MAP_EDITOR_HPP_

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "InputListener.hpp"
#include "PauseCaller.hpp"
#include "PauseListener.hpp"
#include "PauseListener.hpp"
#include "MapPack.hpp"
#include "EditBoxes/EditBoxListener.hpp"

class CEntity;
class CSprite;
class CLink;
class CMap;
class CEditBoxBase;
class CChangeTileEvent;

class CMapEditor 
  : public Ogre::Singleton<CMapEditor>,
    public CInputListener,
    public CPauseCaller,
    public CPauseListener,
    public CEditBoxListener {
public:
  enum EEditButtonTypes {
    EBT_STRING,
    EBT_TEXT,
    EBT_FLOAT,
    EBT_BOOL,
    EBT_UINT_VECTOR2,
    EBT_TILE_TYPE,
    EBT_ENUM_SWEEP,
    EBT_CUSTOM,
  };
  enum EEditButtons {
    EB_MAP_NAME,
    EB_MAP_FILENAME,
    EB_MAP_DIFFICULTY,
    EB_MAP_DESCRIPTION,
    EB_MAP_EXIT,
    EB_MAP_SIZE,
    EB_HITPOINTS,
    EB_DAMAGE,
    EB_JUMPING,
    EB_ID,
    EB_SWITCH_AFFECTING_BLOCKS,
    EB_SWITCH_ENTRY_POSITION,
    EB_SWITCH_ENTRY_TILE_TYPE,
    EB_SWITCH_DEACTIVABLE,
    EB_SWITCH_TIMED,
    EB_SWITCH_ACTIVATION_TIME,
    EB_LINK_FROM,
    EB_LINK_TO,
    EB_TILE_ENDANGERED,
  };
  enum EBrushes {
    B_PLACE,
    B_EDIT,
    B_MOVE,

    B_COUNT,
  };
private:
  // buffer variables for edit boxes
  bool m_bBoolBuffer;

  // buttons
  bool m_bMiddleMouseButtonPressed;
  bool m_bShiftPressed;

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
  CEntity *m_pSelectedEntity;
  bool m_bSnapToGrid;
  CMapPackPtr m_pMapPack;
  bool m_bInitialized;
  
  bool m_bGrabbedSprite;

  unsigned int m_uiMapSizeX;
  unsigned int m_uiMapSizeY;

  Ogre::Vector2 m_vClickPos;
  
public:
  static CMapEditor &getSingleton();
  static CMapEditor *getSingletonPtr();

  CMapEditor(CEGUI::Window *pRoot);


  void toggle();
  void init(CMap *pMap, const CMapPackConstPtr pMapPack);
  void start();
  void stop();
  void exit();
  void render();

  bool isVisible() const {return m_bVisible;}

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
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  void resize(float fButtonSize);
  void setVisible(bool bVisible);
  void reloadTextures();
  void mapEditorUpdatePauseChanged(bool bPause);
private:
  CEGUI::ListboxTextItem *createLinkEntry(const CLink &link);
  CEGUI::ListboxTextItem *createSwitchEntry(const CChangeTileEvent &entry);
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
  void placeCurrentTile(int x, int y);
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
  bool onDeleteLink(const CEGUI::EventArgs &args);
  bool onAddLink(const CEGUI::EventArgs &args);

  bool onEditEnumSweep(const CEGUI::EventArgs&args);
  bool onEditText(const CEGUI::EventArgs &args);
  bool onEditString(const CEGUI::EventArgs &args);
  bool onEditFloat(const CEGUI::EventArgs &args);
  bool onEditUIntVector2(const CEGUI::EventArgs &args);
  bool onEditBoolChanged(const CEGUI::EventArgs &args);
  bool onEditTileType(const CEGUI::EventArgs &args);
  bool onEditCustom(const CEGUI::EventArgs &args);
  bool dummyReturnFalse(const CEGUI::EventArgs &args) {return false;}

  void editTile(const Ogre::Vector2 &vPos);
  CEntity *getEntityAtMousePos(const Ogre::Vector2 &vPos);
  bool selectEntity(const Ogre::Vector2 &vPos);
  void selectedEntity(CEntity *pEntity);

  void onEditBoxAccept(CEditBoxBase *);
  void onEditBoxCancel(CEditBoxBase *);
};

#endif
