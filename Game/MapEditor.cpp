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

#include "MapEditor.hpp"
#include "TileTypes.hpp"
#include "HUD.hpp"
#include "Tile.hpp"
#include "Map.hpp"
#include "Enemy.hpp"
#include "Game.hpp"
#include "Switch.hpp"
#include "Object.hpp"
#include "Link.hpp"
#include "DebugDrawer.hpp"
#include "Player.hpp"
#include "FileManager.hpp"
#include "Settings.hpp"
#include "GUIManager.hpp"
#include "IDGenerator.hpp"
#include "EditBoxes/EditBoxFloat.hpp"
#include "EditBoxes/EditBoxUIntVector2.hpp"
#include "EditBoxes/EditBoxTileType.hpp"
#include "EditBoxes/EditBoxString.hpp"
#include "EditBoxes/EditBoxText.hpp"
#include "EditBoxes/EditBoxExit.hpp"
#include "Events/ChangeTileEvent.hpp"
#include "IDGenerator.hpp"
#include "MapInfo.hpp"

using namespace CEGUI;

template<> CMapEditor *Ogre::Singleton<CMapEditor>::msSingleton = 0;

CMapEditor *CMapEditor::getSingletonPtr() {
  return msSingleton;
}
CMapEditor &CMapEditor::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

const CEGUI::String MAP_EDITOR_BRUSH_LABELS[CMapEditor::B_COUNT] = {
  "Place",
  "Edit",
  "Move",
};

CMapEditor::CMapEditor(Window *pRoot) 
  : m_bMiddleMouseButtonPressed(false),
    m_bShiftPressed(false),
    m_bRenderPause(false),
    m_bVisible(false),
    m_pRoot(pRoot),
    m_pTabControl(NULL),
    m_pEditValueWindow(NULL),
    m_uiCurrentTile(2),
    m_pMap(NULL),
    m_pSelectedEntity(NULL),
    m_bInitialized(false),
    m_uiMapSizeX(0),
    m_uiMapSizeY(0) {
  //init(0);
}
void CMapEditor::init(CMap *pMap, const CMapPackConstPtr pMapPack) {
  for (unsigned int i = 0; i < TT_COUNT; i++) {
    String tileName = "Tile" + PropertyHelper<int>::toString(i) + ".png";
    ImageManager::getSingleton().addFromImageFile(tileName, tileName, "Game");
  }
  for (unsigned int i = 0; i < CObject::OT_COUNT; i++) {
    String objectName = CObject::getPreviewImageName(i);
    ImageManager::getSingleton().addFromImageFile(objectName, objectName, "Game");
  }
  for (unsigned int i = 0; i < CEnemy::ET_COUNT; i++) {
    String objectName = CEnemy::getPreviewImageName(i);
    ImageManager::getSingleton().addFromImageFile(objectName, objectName, "Game");
  }
  for (unsigned int i = 0; i < CSwitch::SWITCH_COUNT; i++) {
    String objectName = CSwitch::getPreviewImageName(i);
    ImageManager::getSingleton().addFromImageFile(objectName, objectName, "Game");
  }

  
  CInputListenerManager::getSingleton().addInputListener(this);
  m_pMap = pMap;
  m_pMapPack = shared_ptr<CMapPack>(new CMapPack(shared_ptr<CMapInfo>(new CMapInfo(pMapPack->getMapInfo()))));
  

  m_uiMapSizeX = m_pMap->getTilesGrid().getSizeX();
  m_uiMapSizeY = m_pMap->getTilesGrid().getSizeY();

  m_bVisible = false;
  m_bInitialized = true;
  
  resize(CSettings::getSingleton().getInputSettings().m_fMapEditorButtonSize);
  stop();
}
void CMapEditor::setVisible(bool bVisible) {
  m_bRenderPause = !bVisible;
  if (m_pTabControl && m_bVisible) {
    m_pTabControl->setVisible(bVisible);
    setInputListenerEnabled(bVisible);
    if (bVisible) {
      pause(PAUSE_MAP_UPDATE);
      CHUD::getSingleton().hide();
    }
    else {
      unpause(PAUSE_MAP_UPDATE);
      CHUD::getSingleton().hide();
    }
  }
}
void CMapEditor::mapEditorUpdatePauseChanged(bool bPause) {
  if (m_bInitialized && m_bVisible) {
    setInputListenerEnabled(!bPause);
    m_pTabControl->setEnabled(!bPause);
    m_pEditValueWindow = NULL;
  }
}
void CMapEditor::resize(float fButtonSize) {
  if (!m_bInitialized) {return;}
  if (m_pTabControl) {
    m_pTabControl->destroy();
  }
  float fDragIgnoreZone = fButtonSize * 0.2;
#ifdef INPUT_TOUCH
  fDragIgnoreZone = CSettings::getSingleton().getInputSettings().m_fTouchButtonSize * 0.2;
#endif
  float fTabSize = 0.3;
  float fTabPixelWidth = CGame::getSingleton().getRenderWindow()->getWidth() * fTabSize;
  int iTilesCountX = static_cast<int>(fTabPixelWidth / fButtonSize - 0.999);
  m_fButtonSize = fButtonSize;

  Ogre::LogManager::getSingleton().logMessage("*** Creating MapEditor");


  Ogre::LogManager::getSingleton().logMessage("    TabControl");
  TabControl *pTabPane = dynamic_cast<TabControl*>(m_pRoot->createChild("OgreTray/TabControl", "TilesGroup"));
  m_pTabControl = pTabPane;
  pTabPane->setPosition(UVector2(UDim(1 - fTabSize, 0), UDim(0, 0)));
  pTabPane->setSize(USize(UDim(fTabSize, 0), UDim(1, 0)));


  // Brush content pane
  // ==================
  Ogre::LogManager::getSingleton().logMessage("    Brush content pane");
  Window *pBrushPane = pTabPane->createChild("OgreTray/TabContentPane", "BrushContentPane");
  //pTabPane->addTab(pBrushPane);
  pBrushPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pBrushPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pBrushPane->setText("Brush");
  pBrushPane->setRiseOnClickEnabled(false);

  ScrollablePane *pBrushScrollPane = dynamic_cast<ScrollablePane*>(pBrushPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pBrushScrollPane->setDragIgnoreZone(fDragIgnoreZone);
  pBrushScrollPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pBrushScrollPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pBrushScrollPane->setRiseOnClickEnabled(false);

  float fCurrentHeight = 0;
  for (int i = 0; i < B_COUNT; i++) {
    RadioButton *pBrushSelection = dynamic_cast<RadioButton*>(pBrushScrollPane->createChild("OgreTray/RadioButton", MAP_EDITOR_BRUSH_LABELS[i]));
    pBrushSelection->setText(MAP_EDITOR_BRUSH_LABELS[i]);
    pBrushSelection->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
    pBrushSelection->setSize(USize(UDim(1, 0), UDim(0, fButtonSize)));
    pBrushSelection->setGroupID(812723);
    pBrushSelection->setID(i);
    pBrushSelection->setSelected(false);
    pBrushSelection->subscribeEvent(RadioButton::EventSelectStateChanged,
				    Event::Subscriber(&CMapEditor::onBrushSelectionChanged, this));
    fCurrentHeight += fButtonSize;
  }
  dynamic_cast<RadioButton*>(pBrushScrollPane->getChild(MAP_EDITOR_BRUSH_LABELS[B_PLACE]))->setSelected(true);

  // offset
  fCurrentHeight += 10;

  // create check buttons
  ToggleButton *pSnapToGridButton = dynamic_cast<ToggleButton*>(pBrushScrollPane->createChild("OgreTray/Checkbox", "SnapToGrid"));
  pSnapToGridButton->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  pSnapToGridButton->setSize(USize(UDim(1, 0), UDim(0, fButtonSize)));
  fCurrentHeight += fButtonSize;
  pSnapToGridButton->setText("Snap to grid");
  pSnapToGridButton->setSelected(false);
  pSnapToGridButton->subscribeEvent(ToggleButton::EventSelectStateChanged,
				    Event::Subscriber(&CMapEditor::onSnapToGridChanged, this));
  pSnapToGridButton->setSelected(true);

  fCurrentHeight += 10; // offset

  createEditButton(pBrushScrollPane, EB_MAP_NAME, EBT_STRING, fCurrentHeight);
  createEditButton(pBrushScrollPane, EB_MAP_DESCRIPTION, EBT_TEXT, fCurrentHeight);
  createEditButton(pBrushScrollPane, EB_MAP_DIFFICULTY, EBT_ENUM_SWEEP, fCurrentHeight);
  createEditButton(pBrushScrollPane, EB_MAP_SIZE, EBT_UINT_VECTOR2, fCurrentHeight);
  createEditButton(pBrushScrollPane, EB_MAP_EXIT, EBT_CUSTOM, fCurrentHeight);

  fCurrentHeight += 10; // offset

  createEditButton(pBrushScrollPane, EB_MAP_FILENAME, EBT_STRING, fCurrentHeight);

  Window *pSaveButton = pBrushScrollPane->createChild("OgreTray/Button", "SaveButton");
  pSaveButton->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  pSaveButton->setSize(USize(UDim(1, 0), UDim(0, fButtonSize)));
  fCurrentHeight += fButtonSize;
  pSaveButton->setText("Save to file");
  pSaveButton->subscribeEvent(PushButton::EventClicked, 
			      Event::Subscriber(&CMapEditor::onSaveMap, this));

  

  // Tiles content pane
  // ==================
  Ogre::LogManager::getSingleton().logMessage("    Tiles content pane");
  Window *pTilesTabContentPane = pTabPane->createChild("OgreTray/TabContentPane", "TilesContentPane");
  //pTabPane->addTab(pTilesTabContentPane);
  pTilesTabContentPane->setRiseOnClickEnabled(false);
  pTilesTabContentPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pTilesTabContentPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pTilesTabContentPane->setText("Tiles");
  pTilesTabContentPane->setFont("dejavusans12");
  
  
  ScrollablePane *pTilesScrollBar = dynamic_cast<ScrollablePane*>(pTilesTabContentPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pTilesScrollBar->setRiseOnClickEnabled(false);
  pTilesScrollBar->setDragIgnoreZone(fDragIgnoreZone);
  m_pTilesContainer = pTilesScrollBar;
  pTilesScrollBar->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pTilesScrollBar->setSize(USize(UDim(1, 0), UDim(1, 0)));

  fCurrentHeight = 0;
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
		       Event::Subscriber(&CMapEditor::onTileClicked, this));

    }
    if (id == TT_COUNT) {
      break;
    }
  }

  // object/enemies content pane
  // ===========================
  Ogre::LogManager::getSingleton().logMessage("    Objects content pane");
  Window *pObjectsTabContentPane = pTabPane->createChild("OgreTray/TabContentPane", "ObjectsContentPane");
  pObjectsTabContentPane->setRiseOnClickEnabled(false);
  //pTabPane->addTab(pTilesTabContentPane);
  pObjectsTabContentPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pObjectsTabContentPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pObjectsTabContentPane->setText("Objects");

  ScrollablePane *pObjectsScrollBar = dynamic_cast<ScrollablePane*>(pObjectsTabContentPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pObjectsScrollBar->setRiseOnClickEnabled(false);
  pObjectsScrollBar->setDragIgnoreZone(fDragIgnoreZone);
  m_pObjectsContainer = pObjectsScrollBar;
  pObjectsScrollBar->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pObjectsScrollBar->setSize(USize(UDim(1, 0), UDim(1, 0)));

  fCurrentHeight = 0;
  id = 0;
  for (int y = 0; y < CObject::OT_COUNT / iTilesCountX + 1; y++) {
    for (int x = 0; x < iTilesCountX; x++) {
      if (id == CObject::OT_COUNT) {break;}
      
      String objectName(CObject::getPreviewImageName(id));
      Window *pObject = pObjectsScrollBar->createChild("OgreTray/StaticImage", PropertyHelper<int>::toString(id));
      pObject->setProperty("FrameEnabled", "False");
      pObject->setProperty("Image", objectName);
      pObject->setProperty("HorzFormatting", "CentreAligned");
      pObject->setProperty("VertFormatting", "CentreAligned");
      pObject->setPosition(UVector2(UDim(0, x * fButtonSize * 1.05f), UDim(0, y * fButtonSize * 1.05f)));
      pObject->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
      pObject->
	subscribeEvent(Window::EventMouseButtonUp,
		       Event::Subscriber(&CMapEditor::onObjectClicked, this));
      
      id++;
    }
    if (id == CObject::OT_COUNT) {break;}
  }
  fCurrentHeight += (CObject::OT_COUNT / iTilesCountX + 1) * fButtonSize;
  int iIdOffset = CObject::OT_COUNT;
  id = 0;
  for (int y = 0; y < CEnemy::ET_COUNT / iTilesCountX + 1; y++) {
    for (int x = 0; x < iTilesCountX; x++) {
      if (id == CEnemy::ET_COUNT) {break;}
      String objectName(CEnemy::getPreviewImageName(id));
      Window *pObject = pObjectsScrollBar->createChild("OgreTray/StaticImage", PropertyHelper<int>::toString(id + iIdOffset));
      pObject->setProperty("FrameEnabled", "False");
      pObject->setProperty("Image", objectName);
      pObject->setProperty("HorzFormatting", "CentreAligned");
      pObject->setProperty("VertFormatting", "CentreAligned");
      pObject->setPosition(UVector2(UDim(0, x * fButtonSize * 1.05f), UDim(0, fCurrentHeight + y * fButtonSize * 1.05f)));
      pObject->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
      pObject->
	subscribeEvent(Window::EventMouseButtonUp,
		       Event::Subscriber(&CMapEditor::onObjectClicked, this));
      
      id++;
    }
    if (id == CEnemy::ET_COUNT) {break;}
  }
  fCurrentHeight += (CEnemy::ET_COUNT / iTilesCountX + 1) * fButtonSize;

  // switches
  iIdOffset += CEnemy::ET_COUNT;
  id = 0;
  for (unsigned int y = 0; y < CSwitch::SWITCH_COUNT / iTilesCountX + 1; y++) {
    for (int x = 0; x < iTilesCountX; x++) {
      if (id == CSwitch::SWITCH_COUNT) {break;}
      String objectName(CSwitch::getPreviewImageName(id));
      Window *pObject = pObjectsScrollBar->createChild("OgreTray/StaticImage", PropertyHelper<int>::toString(id + iIdOffset));
      pObject->setProperty("FrameEnabled", "False");
      pObject->setProperty("Image", objectName);
      pObject->setProperty("HorzFormatting", "CentreAligned");
      pObject->setProperty("VertFormatting", "CentreAligned");
      pObject->setPosition(UVector2(UDim(0, x * fButtonSize * 1.05f), UDim(0, fCurrentHeight + y * fButtonSize * 1.05f)));
      pObject->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
      pObject->
	subscribeEvent(Window::EventMouseButtonUp,
		       Event::Subscriber(&CMapEditor::onObjectClicked, this));
      
      id++;
    }
    if (id == CSwitch::SWITCH_COUNT) {break;}
  }
  fCurrentHeight += (CSwitch::SWITCH_COUNT / iTilesCountX + 1) * fButtonSize;

  // edit tab
  // ========
  Ogre::LogManager::getSingleton().logMessage("    Edit content pane");
  Window *pEditTabContentPane = pTabPane->createChild("OgreTray/TabContentPane", "EditContentPane");
  //pTabPane->addTab(pTilesTabContentPane);
  pEditTabContentPane->setRiseOnClickEnabled(false);
  pEditTabContentPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pEditTabContentPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pEditTabContentPane->setText("Edit");


  ScrollablePane *pEditScrollBar = dynamic_cast<ScrollablePane*>(pEditTabContentPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pEditScrollBar->setRiseOnClickEnabled(false);
  pEditScrollBar->setDragIgnoreZone(fDragIgnoreZone);
  m_pEditContainer = pEditScrollBar;
  pEditScrollBar->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pEditScrollBar->setSize(USize(UDim(1, 0), UDim(1, 0)));

  fCurrentHeight = 0;
  Window *pDeleteButton = pEditScrollBar->createChild("OgreTray/Button", "Delete");
  pDeleteButton->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  pDeleteButton->setSize(USize(UDim(1, 0), UDim(0, fButtonSize)));
  fCurrentHeight += fButtonSize + 5;
  pDeleteButton->setText("Delete selection");
  pDeleteButton->subscribeEvent(PushButton::EventClicked,
				Event::Subscriber(&CMapEditor::onDelete, this));
  createEditButton(pEditScrollBar, EB_ID, EBT_STRING, fCurrentHeight);
  
  
  // edit sprite
  m_pEditSprite = pEditScrollBar->createChild("DefaultWindow", "Sprite");
  m_pEditSprite->setRiseOnClickEnabled(false);
  m_pEditSprite->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  fCurrentHeight = 5;
  createEditButton(m_pEditSprite, EB_HITPOINTS, EBT_FLOAT, fCurrentHeight);
  //createEditButton(m_pEditSprite, EB_DAMAGE, EBT_FLOAT, fCurrentHeight);
  createEditButton(m_pEditSprite, EB_JUMPING, EBT_BOOL, fCurrentHeight);
  
  // switch
  Window *pEditSwitchPane = pEditScrollBar->createChild("DefaultWindow", "Switch");
  pEditSwitchPane->setRiseOnClickEnabled(false);
  m_pEditSwitchPane = pEditSwitchPane;
  fCurrentHeight = fButtonSize + 5;
  pEditSwitchPane->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  fCurrentHeight = 5;
  createEditButton(pEditSwitchPane, EB_SWITCH_AFFECTING_BLOCKS, EBT_BOOL, fCurrentHeight);
  Window *pAffectingBlocksList = pEditSwitchPane->createChild("OgreTray/Listbox", "List");
  pAffectingBlocksList->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  pAffectingBlocksList->setSize(USize(UDim(1, -fButtonSize), UDim(0, 2 * fButtonSize)));
  Window *pAddAB = pEditSwitchPane->createChild("OgreTray/Button", "Add");
  pAddAB->setPosition(UVector2(UDim(1, -fButtonSize), UDim(0, fCurrentHeight)));
  pAddAB->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
  pAddAB->setText("+");
  pAddAB->subscribeEvent(PushButton::EventClicked,
			 Event::Subscriber(&CMapEditor::onAddSwitchEntry, this));
  Window *pRemoveAB = pEditSwitchPane->createChild("OgreTray/Button", "Remove");
  pRemoveAB->setPosition(UVector2(UDim(1, -fButtonSize), UDim(0, fButtonSize + fCurrentHeight)));
  pRemoveAB->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
  pRemoveAB->setText("-");
  pRemoveAB->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onDeleteSwitchEntry, this));
  fCurrentHeight += 2 * fButtonSize;

  createEditButton(pEditSwitchPane, EB_SWITCH_ENTRY_POSITION, EBT_UINT_VECTOR2, fCurrentHeight);
  createEditButton(pEditSwitchPane, EB_SWITCH_ENTRY_TILE_TYPE, EBT_TILE_TYPE, fCurrentHeight);
  createEditButton(pEditSwitchPane, EB_SWITCH_DEACTIVABLE, EBT_BOOL, fCurrentHeight);
  createEditButton(pEditSwitchPane, EB_SWITCH_TIMED, EBT_BOOL, fCurrentHeight);
  createEditButton(pEditSwitchPane, EB_SWITCH_ACTIVATION_TIME, EBT_FLOAT, fCurrentHeight);


  // links tab
  // ========
  fCurrentHeight = 0;
  Ogre::LogManager::getSingleton().logMessage("    Link content pane");
  Window *pLinkTabContentPane = pTabPane->createChild("OgreTray/TabContentPane", "LinkContentPane");
  pLinkTabContentPane->setRiseOnClickEnabled(false);
  pLinkTabContentPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pLinkTabContentPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pLinkTabContentPane->setText("Link");


  ScrollablePane *pLinkScrollBar = dynamic_cast<ScrollablePane*>(pLinkTabContentPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pLinkScrollBar->setRiseOnClickEnabled(false);
  pLinkScrollBar->setDragIgnoreZone(fDragIgnoreZone);
  pLinkScrollBar->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pLinkScrollBar->setSize(USize(UDim(1, 0), UDim(1, 0)));

  Listbox *pLinksList = dynamic_cast<Listbox*>(pLinkScrollBar->createChild("OgreTray/Listbox", "List"));
  pLinksList->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  pLinksList->setSize(USize(UDim(1, -fButtonSize), UDim(0, 2 * fButtonSize)));
  pAddAB = pLinkScrollBar->createChild("OgreTray/Button", "Add");
  pAddAB->setPosition(UVector2(UDim(1, -fButtonSize), UDim(0, fCurrentHeight)));
  pAddAB->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
  pAddAB->setText("+");
  pAddAB->subscribeEvent(PushButton::EventClicked,
			 Event::Subscriber(&CMapEditor::onAddLink, this));
  pRemoveAB = pLinkScrollBar->createChild("OgreTray/Button", "Remove");
  pRemoveAB->setPosition(UVector2(UDim(1, -fButtonSize), UDim(0, fButtonSize + fCurrentHeight)));
  pRemoveAB->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
  pRemoveAB->setText("-");
  pRemoveAB->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onDeleteLink, this));
  fCurrentHeight += 2 * fButtonSize;

  createEditButton(pLinkScrollBar, EB_LINK_FROM, EBT_UINT_VECTOR2, fCurrentHeight);
  createEditButton(pLinkScrollBar, EB_LINK_TO, EBT_UINT_VECTOR2, fCurrentHeight);

  // initial state
  // =============
  selectTile(1);
}
Window* CMapEditor::createEditButton(Window *pParent,
				     EEditButtons id,
				     EEditButtonTypes type,
				     float &fCurrentHeight) {
  Window *pButton;
  switch (type) {
  case EBT_BOOL:
    pButton = pParent->createChild("OgreTray/Checkbox", PropertyHelper<int>::toString(id));
    break;
  default:
    pButton = pParent->createChild("OgreTray/Button", PropertyHelper<int>::toString(id));
    break;
  }
  pButton->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  pButton->setSize(USize(UDim(1, 0), UDim(0, m_fButtonSize)));
  fCurrentHeight += m_fButtonSize + 5;
  switch (type) {
  case EBT_ENUM_SWEEP:
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onEditEnumSweep, this));
    break;
  case EBT_FLOAT:
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onEditFloat, this));
    break;
  case EBT_TEXT:
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onEditText, this));
    break;    
  case EBT_STRING:
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onEditString, this));
    break;
  case EBT_BOOL:
    pButton->subscribeEvent(ToggleButton::EventSelectStateChanged,
			    Event::Subscriber(&CMapEditor::onEditBoolChanged, this));
    break;
  case EBT_UINT_VECTOR2:
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onEditUIntVector2, this));
    break;
  case EBT_TILE_TYPE:
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onEditTileType, this));
    break;
  case EBT_CUSTOM:
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CMapEditor::onEditCustom, this));
    break;
  default:
    break;
  }
  switch (id) {
  case EB_MAP_FILENAME:
    pButton->setText("Map file name");
    break;
  case EB_MAP_DESCRIPTION:
    pButton->setText("Map description");
    break;
  case EB_MAP_DIFFICULTY:
    pButton->setText(m_pMapPack->getMapInfo()->getDifficultyAsString().c_str());
    break;
  case EB_MAP_NAME:
    pButton->setText("Map name");
    break;
  case EB_MAP_SIZE:
    pButton->setText("Map size");
    break;
  case EB_MAP_EXIT:
    pButton->setText("Victory condition");
    break;
  case EB_HITPOINTS:
    pButton->setText("Edit Hitpoints");
    break;
  case EB_DAMAGE:
    pButton->setText("Edit Damage");
    break;
  case EB_JUMPING:
    pButton->setText("Jumping");
    break;
  case EB_ID:
    pButton->setText("ID");
    break;
  case EB_SWITCH_AFFECTING_BLOCKS:
    pButton->setText("Affecting blocks");
    break;
  case EB_SWITCH_ENTRY_POSITION:
    pButton->setText("Edit position");
    break;
  case EB_SWITCH_ENTRY_TILE_TYPE:
    pButton->setText("Edit tile type");
    break;
  case EB_SWITCH_DEACTIVABLE:
    pButton->setText("Deactivable");
    break;
  case EB_SWITCH_TIMED:
    pButton->setText("Timed");
    break;
  case EB_SWITCH_ACTIVATION_TIME:
    pButton->setText("Edit activation duration");
    break;
  case EB_LINK_FROM:
    pButton->setText("First");
    break;
  case EB_LINK_TO:
    pButton->setText("Second");
    break;
  default:
    break;
  }
  return pButton;
}
void CMapEditor::reloadTextures() {
  if (!m_bInitialized) {return;}

  for (unsigned int i = 0; i < TT_COUNT; i++) {
    String tileName = "Tile" + PropertyHelper<int>::toString(i) + ".png";
    CGUIManager::getSingleton().getRenderer()->getTexture(tileName).
      loadFromFile(tileName, "Game");
  }
  for (unsigned int i = 0; i < CObject::OT_COUNT; i++) {
    String objectName = CObject::getPreviewImageName(i);
    CGUIManager::getSingleton().getRenderer()->getTexture(objectName).
      loadFromFile(objectName, "Game");
  }
  for (unsigned int i = 0; i < CEnemy::ET_COUNT; i++) {
    String objectName = CEnemy::getPreviewImageName(i);
    CGUIManager::getSingleton().getRenderer()->getTexture(objectName).
      loadFromFile(objectName, "Game");
  }
  for (unsigned int i = 0; i < CSwitch::SWITCH_COUNT; i++) {
    String objectName = CSwitch::getPreviewImageName(i);
    CGUIManager::getSingleton().getRenderer()->getTexture(objectName).
      loadFromFile(objectName, "Game");
  }
  
}
void CMapEditor::exit() {
  Ogre::LogManager::getSingleton().logMessage("MapEditor exit ...");
  for (unsigned int i = 0; i < TT_COUNT; i++) {
    String tileName = "Tile" + PropertyHelper<int>::toString(i) + ".png";
    ImageManager::getSingleton().destroy(tileName);
    CGUIManager::getSingleton().getRenderer()->destroyTexture(tileName);
  }
  for (unsigned int i = 0; i < CObject::OT_COUNT; i++) {
    String objectName = CObject::getPreviewImageName(i);
    ImageManager::getSingleton().destroy(objectName);
    CGUIManager::getSingleton().getRenderer()->destroyTexture(objectName);
  }
  for (unsigned int i = 0; i < CEnemy::ET_COUNT; i++) {
    String objectName = CEnemy::getPreviewImageName(i);
    ImageManager::getSingleton().destroy(objectName);
    CGUIManager::getSingleton().getRenderer()->destroyTexture(objectName);
  }
  for (unsigned int i = 0; i < CSwitch::SWITCH_COUNT; i++) {
    String objectName = CSwitch::getPreviewImageName(i);
    ImageManager::getSingleton().destroy(objectName);
    CGUIManager::getSingleton().getRenderer()->destroyTexture(objectName);
  }

  cout << ImageManager::getSingleton().getImageCount() << endl;
  stop();
  m_bInitialized = false;
  CInputListenerManager::getSingleton().removeInputListener(this);
  m_pTabControl->destroy();

  m_uiCurrentTile = 2;
  m_pMap = NULL;
  m_pTabControl = NULL;
  m_pSelectedEntity = NULL;
  m_bVisible = false;
  m_bRenderPause = false;
  m_bInitialized = false;
  if (m_pEditValueWindow) {
    delete m_pEditValueWindow;
    m_pEditValueWindow = NULL;
  }
  Ogre::LogManager::getSingleton().logMessage("   ... done");
}
void CMapEditor::start() {
  // reset buttons
  m_bShiftPressed = false;
  m_bMiddleMouseButtonPressed = false;

  setInputListenerEnabled(true);
  pause(PAUSE_MAP_UPDATE);
  CHUD::getSingleton().hide();
  m_bVisible = true;
  m_pTabControl->setVisible(true);
  m_bPressed = false;
  Sizef vSize(m_pTabControl->getPixelSize());
  m_pMap->resize(Ogre::Vector2(CGame::getSingleton().getRenderWindow()->getWidth() - vSize.d_width, vSize.d_height));
  selectedEntity(NULL);
  
  //reset map, but store camera pos
  Ogre::Vector2 vCamPos(m_pMap->getCameraTargetPos());
  m_pMap->loadMap(m_pMapPack);
  m_pMap->setCameraPos(vCamPos);

  // init links
  Listbox *pLinksList = dynamic_cast<Listbox*>(m_pTabControl->getChild("LinkContentPane")->getChild("ScrollPane")->getChild("List"));
  while (pLinksList->getItemCount() > 0) {
    pLinksList->removeItem(pLinksList->getListboxItemFromIndex(0));
  }
  for (const CEntity *pLink : m_pMap->getLinks()) {
    pLinksList->addItem(createLinkEntry(*dynamic_cast<const CLink*>(pLink)));
  }
  // init map size
  m_uiMapSizeX = m_pMap->getTilesGrid().getSizeX();
  m_uiMapSizeY = m_pMap->getTilesGrid().getSizeY();
}
void CMapEditor::stop() {
  selectedEntity(NULL);
  setInputListenerEnabled(false);
  unpause(PAUSE_MAP_UPDATE);
  CHUD::getSingleton().show();
  m_pTabControl->setVisible(false);
  m_pMap->resize(Ogre::Vector2(CGame::getSingleton().getRenderWindow()->getWidth(),
			       CGame::getSingleton().getRenderWindow()->getHeight()));

  m_pMap->prepareMap();
  if (m_bVisible) {
    m_pMap->writeToXMLElement(m_pMapPack->getMapInfo()->getEmptyRootNode(), OS_FULL);
    m_bVisible = false;
  }
}
void CMapEditor::render() {
  if (!m_bVisible || m_bRenderPause) {return;}
  if (m_pSelectedEntity) {
    CDebugDrawer::getSingleton().draw(m_pSelectedEntity->getWorldBoundingBox());
    CSwitch *pSwitch = dynamic_cast<CSwitch*>(m_pSelectedEntity);
    if (pSwitch) {
      int iCounter = 0;
      int iSelected = -1;
      Listbox *pLB = dynamic_cast<Listbox*>(m_pEditSwitchPane->getChild("List"));
      if (pLB->getFirstSelectedItem()) {
	iSelected = pLB->getItemIndex(pLB->getFirstSelectedItem());
      }
      
      for (const CEvent *pEvent : pSwitch->getEvents()) {
	CDebugDrawer::getSingleton().draw(pEvent, (iCounter == iSelected) ? 0.9 : 0.5);
	iCounter++;
      }
    }
  }
  Listbox *pLinksList = dynamic_cast<Listbox*>(m_pTabControl->getChild("LinkContentPane")->getChild("ScrollPane")->getChild("List"));
  if (pLinksList->getFirstSelectedItem()) {
    CDebugDrawer::getSingleton().draw(*static_cast<CLink*>(pLinksList->getFirstSelectedItem()->getUserData()));
  }
  m_pMap->getExit().debugDraw();
}
void CMapEditor::toggle() {
  if (m_bVisible) {stop();}
  else {start();}
}
bool CMapEditor::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_TAB || arg.key == OIS::KC_RWIN) {
    stop();
  }
  else if (arg.key == OIS::KC_LSHIFT) {
    m_bShiftPressed = true;
  }
  return true;
}
bool CMapEditor::keyReleased( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_LSHIFT) {
    m_bShiftPressed = false;
  }
  return true;
}
CEGUI::Window *CMapEditor::getTileFromType(unsigned int uiTile) {
  String tileName = "Tile" + PropertyHelper<int>::toString(uiTile) + ".png";
  return m_pTilesContainer->getChild(tileName);
}
unsigned int CMapEditor::getTypeFromTile(CEGUI::Window *pTile) {
  return PropertyHelper<int>::fromString(pTile->getName().substr(4, pTile->getName().size() - 8));
}
void CMapEditor::selectTile(unsigned int uiTile) {
  if (m_uiCurrentTile != TT_COUNT) {
    getTileFromType(m_uiCurrentTile)->setProperty("FrameEnabled", "False");
  }
  if (uiTile != TT_COUNT) {
    getTileFromType(uiTile)->setProperty("FrameEnabled", "True");
    selectObject(CObject::OT_COUNT + CEnemy::ET_COUNT + CSwitch::SWITCH_COUNT);
  }
  m_uiCurrentTile = uiTile;
}
bool CMapEditor::onTileClicked(const EventArgs &args) {
  if (m_pTilesContainer->wasDragged()) {return true;}
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  selectTile(getTypeFromTile(wndArgs.window));

  return true;
}
CEGUI::Window *CMapEditor::getObjectFromType(unsigned int uiObject) {
  return m_pObjectsContainer->getChild(PropertyHelper<int>::toString(uiObject));
}
unsigned int CMapEditor::getTypeFromObject(CEGUI::Window *pObject) {
  return PropertyHelper<int>::fromString(pObject->getName());
}
void CMapEditor::selectObject(unsigned int uiObject) {
  if (m_uiCurrentObject < CObject::OT_COUNT + CEnemy::ET_COUNT + CSwitch::SWITCH_COUNT) {
    getObjectFromType(m_uiCurrentObject)->setProperty("FrameEnabled", "False");
  }
  if (uiObject < CObject::OT_COUNT + CEnemy::ET_COUNT + CSwitch::SWITCH_COUNT) {
    getObjectFromType(uiObject)->setProperty("FrameEnabled", "True");
    selectTile(TT_COUNT);
  }
  m_uiCurrentObject = uiObject;
}
bool CMapEditor::onObjectClicked(const EventArgs &args) {
  if (m_pTilesContainer->wasDragged()) {return true;}
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  selectObject(getTypeFromObject(wndArgs.window));

  return true;
}
bool CMapEditor::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  if (id == OIS::MB_Middle) {
    // always move map when middle mouse button pressed
    m_bMiddleMouseButtonPressed = true;
    return true;
  }
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs)));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return true;}

  // only pressed when clicked on map!
  m_bPressed = true;
  handleBrushPressed(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
  return true;
}
bool CMapEditor::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  if (id == OIS::MB_Middle) {
    m_bMiddleMouseButtonPressed = false;
    return true;
  }
  if (m_bPressed) {
    handleBrushReleased(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
  }
  m_bPressed = false;
  return true;
}
bool CMapEditor::mouseMoved( const OIS::MouseEvent &arg ) {
  if (!m_bPressed && !m_bMiddleMouseButtonPressed) {return true;}

  handleBrushMoved(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs),
		   Ogre::Vector2(arg.state.X.rel, arg.state.Y.rel));
  return true;
}
bool CMapEditor::touchMoved(const OIS::MultiTouchEvent& evt) {
  if (!m_bPressed) {return true;}

  
  handleBrushMoved(Ogre::Vector2(evt.state.X.abs, evt.state.Y.abs),
		   Ogre::Vector2(evt.state.X.rel, evt.state.Y.rel));
  return true;
}
bool CMapEditor::touchPressed(const OIS::MultiTouchEvent& arg) {
 Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs)));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return true;}

  // only pressed when clicked on map!
  m_bPressed = true;
  handleBrushPressed(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
  return true;
}
bool CMapEditor::touchReleased(const OIS::MultiTouchEvent& arg) {
if (m_bPressed) {
    handleBrushReleased(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
  }
  m_bPressed = false;
  return true;
}
bool CMapEditor::touchCancelled(const OIS::MultiTouchEvent& arg) {
  return touchReleased(arg);
}
CEntity *CMapEditor::getEntityAtMousePos(const Ogre::Vector2 &vPos) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(vPos));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return NULL;}

  list<CEntity*> lEntities;

  if (m_pMap->getPlayer()->getWorldBoundingBox().contains(vMapPos)) {
    lEntities.push_back(m_pMap->getPlayer());
  }
  for (CEntity *pSprite : m_pMap->getEnemies()) {
    if (pSprite->getWorldBoundingBox().contains(vMapPos)) {
      lEntities.push_back(pSprite);
    }
  }
  for (CEntity *pSprite : m_pMap->getObjects()) {
    if (pSprite->getWorldBoundingBox().contains(vMapPos)) {
      lEntities.push_back(pSprite);
    }
  }
  for (CEntity *pSprite : m_pMap->getSwitches()) {
    if (pSprite->getWorldBoundingBox().contains(vMapPos)) {
      lEntities.push_back(pSprite);
    }
  }

  CEntity *pSelectedSprite = NULL;
  if (lEntities.size() > 0) {
    pSelectedSprite = lEntities.front();
  }
  return pSelectedSprite;
}
bool CMapEditor::selectEntity(const Ogre::Vector2 &vPos) {
  selectedEntity(getEntityAtMousePos(vPos));
  return true;
}
void CMapEditor::editTile(const Ogre::Vector2 &vPos) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(vPos));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return;}

  int x = static_cast<int>(vMapPos.x);
  int y = static_cast<int>(vMapPos.y);

  if (m_pMap->outOfMap(x, y)) {return;}

  m_pEditValueWindow = new CEditBoxTileType(EB_TILE_ENDANGERED,
					    m_pRoot,
					    m_fButtonSize,
					    "Select endangered tile",
					    m_pMap->getTile(x, y)->getEndangeredTileType(),
					    true,
					    m_pMap->getTile(x, y));
  m_pEditValueWindow->setListener(this);
}
void CMapEditor::handleBrushPressed(const Ogre::Vector2 &vPos) {
  m_vClickPos = vPos;
  switch (m_eSelectedBrush) {
  case B_PLACE:
    if (m_uiCurrentObject < CObject::OT_COUNT + CEnemy::ET_COUNT + CSwitch::SWITCH_COUNT) {
      placeCurrentObject(vPos);
    }
    break;
  case B_MOVE:
    if (m_pSelectedEntity &&
	m_pSelectedEntity->getWorldBoundingBox().contains(m_pMap->mouseToMapPos(vPos))) {
      m_bGrabbedSprite = true;
    }
    else m_bGrabbedSprite = false;
    break;
  case B_EDIT:
    break;
  default:
    break;
  }
  handleBrushMoved(vPos, Ogre::Vector2::ZERO);
}
void CMapEditor::handleBrushMoved(const Ogre::Vector2 &vPos, const Ogre::Vector2 &vDelta) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapSize(vDelta));

  if (m_bMiddleMouseButtonPressed) {
    m_pMap->translateCamera(-vMapPos);
  }
  else {
    switch (m_eSelectedBrush) {
    case B_PLACE:
      if (m_uiCurrentTile != TT_COUNT) {
        placeCurrentTile(vPos);
      }
      break;
    case B_MOVE:
      if (m_pSelectedEntity && m_bGrabbedSprite) {
	m_pSelectedEntity->translate(vMapPos);
      }
      else {
	// move map
	m_pMap->translateCamera(-vMapPos);
      }
      break;
    default:
      break;
    }
  }
}
void CMapEditor::handleBrushReleased(const Ogre::Vector2 &vPos) {
  bool bClick = m_vClickPos.squaredDistance(vPos) < m_fButtonSize * m_fButtonSize * 0.25;
  switch (m_eSelectedBrush) {
  case B_PLACE:
    break;
  case B_MOVE:
    if (m_pSelectedEntity && m_bGrabbedSprite) {
      m_pSelectedEntity->setPosition(snappedPos(m_pSelectedEntity->getPosition()));
    }
    else if (bClick) {
      selectEntity(vPos);
    }
    break;
  case B_EDIT:
    if (bClick) {
      if (!m_pSelectedEntity) {
	selectEntity(vPos);
	if (!m_pSelectedEntity) {
	  editTile(vPos);
	}
      }
      else {
	m_pSelectedEntity = NULL;
	editTile(vPos);
      }
    }
    break;
  default:
    break;
  }
}
void CMapEditor::placeCurrentTile(const Ogre::Vector2 &vPos) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(vPos));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return;}

  int origx = static_cast<int>(vMapPos.x);
  int origy = static_cast<int>(vMapPos.y);

  if (m_bShiftPressed) {
    for (int x = -1; x <= 1; x++) {
      for (int y = -1; y <= 1; y++) {
	placeCurrentTile(origx + x, origy + y);
      }
    }
  }
  else {
    placeCurrentTile(origx, origy);
  }
}
void CMapEditor::placeCurrentTile(int x, int y) {
  if (m_pMap->outOfMap(x, y)) {return;}

  if (m_uiCurrentTile < TT_COUNT) {
    m_pMap->changeTileType(x, y, m_uiCurrentTile);
  }
}
void CMapEditor::placeCurrentObject(const Ogre::Vector2 &vPos) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(vPos));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return;}

  int x = static_cast<int>(vMapPos.x);
  int y = static_cast<int>(vMapPos.y);

  if (m_pMap->outOfMap(x, y)) {return;}

  if (m_uiCurrentObject < CObject::OT_COUNT) {
    CObject *pObject = new CObject(*m_pMap,
				   CIDGenerator::nextID("Object_"),
				   m_pMap->getObjectsEntity(),
				   vMapPos,
				   static_cast<CObject::EObjectTypes>(m_uiCurrentObject));
    // set center to curser pos
    pObject->setCenter(snappedPos(vMapPos));
    selectedEntity(pObject);
  }
  else if (m_uiCurrentObject < CEnemy::ET_COUNT + CObject::OT_COUNT) {
    int id = m_uiCurrentObject - CObject::OT_COUNT;
    CEnemy *pEnemy = new CEnemy(*m_pMap,
				m_pMap->getEnemiesEntity(), 
				vMapPos,
				static_cast<CEnemy::EEnemyTypes>(id), 1, 1, true);
    pEnemy->setCenter(snappedPos(vMapPos));
    selectedEntity(pEnemy);
  }
  else if (m_uiCurrentObject < CEnemy::ET_COUNT + CObject::OT_COUNT + CSwitch::SWITCH_COUNT) {
    int id = m_uiCurrentObject - CObject::OT_COUNT - CEnemy::ET_COUNT;
    CSwitch *pSwitch = new CSwitch(*m_pMap,
				   CIDGenerator::nextID("Switch_"),
				   m_pMap->getSwitchesEntity(),
				   vMapPos,
				   static_cast<CSwitch::ESwitchTypes>(id),
				   false,
				   CSwitch::SS_DEACTIVATED);
    pSwitch->setCenter(snappedPos(vMapPos));
    selectedEntity(pSwitch);
  }
}
bool CMapEditor::onBrushSelectionChanged(const EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  RadioButton *pRB = dynamic_cast<RadioButton*>(wndArgs.window);
  if (pRB->isSelected()) {
    m_eSelectedBrush = static_cast<EBrushes>(pRB->getID());
  }
  return true;
}
bool CMapEditor::onSnapToGridChanged(const EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  ToggleButton *pCB = dynamic_cast<ToggleButton*>(wndArgs.window);
  m_bSnapToGrid = pCB->isSelected();
  return true;
}
bool CMapEditor::onSaveMap(const EventArgs &args) {
  selectedEntity(NULL);

  // write file to map info
  tinyxml2::XMLElement *pElem = m_pMapPack->getMapInfo()->getEmptyRootNode();
  m_pMap->writeToXMLElement(pElem, OS_MINIMAL);
  m_pMapPack->getMapInfo()->writeToXMLElement(pElem, OS_MINIMAL);
  tinyxml2::XMLPrinter printer;
  m_pMapPack->getMapInfo()->getDocument().Accept(&printer);

  fstream stream;
  if (!CFileManager::openFile(stream, CFileManager::DIRECTORY_LEVEL + m_pMapPack->getMapInfo()->getFileName(), std::ofstream::out | std::ofstream::trunc, CFileManager::SL_EXTERNAL)) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Could not open stream for saving the map!");
    return true;
  }

  stream << printer.CStr();
  stream.close();
  Ogre::LogManager::getSingleton().logMessage("Map was saved!");
  

  return true;
}
bool CMapEditor::onDelete(const CEGUI::EventArgs &args) {
  if (!m_pSelectedEntity) {return true;}

  CEntity *pBuffer = m_pSelectedEntity;
  selectedEntity(NULL);

  if (pBuffer->getID() != "Player") {
    // dont delete the player
    delete pBuffer;
  }
  

  return true;
}
bool CMapEditor::onEditEnumSweep(const CEGUI::EventArgs&args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  switch(id) {
  case EB_MAP_DIFFICULTY:
    {
      int diff = m_pMapPack->getMapInfo()->getDifficulty() + 1;
      if (diff >= D_COUNT || diff < 0) {
	diff = 0;
      }
      m_pMapPack->getMapInfo()->setDifficulty(static_cast<EMapDifficulty>(diff));
      wndArgs.window->setText(m_pMapPack->getMapInfo()->getDifficultyAsString().c_str());
    }
    break;
  }
  return true; 
}
bool CMapEditor::onEditText(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  switch(id) {
  case EB_MAP_DESCRIPTION:
    m_pEditValueWindow = new CEditBoxText(id,
					  m_pRoot,
					  m_fButtonSize,
					  wndArgs.window->getText(),
					  m_pMapPack->getMapInfo()->getDescription());
    break;
  }
  return true;  
}
bool CMapEditor::onEditString(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  switch(id) {
  case EB_MAP_NAME:
    m_pEditValueWindow = new CEditBoxString(id,
					    m_pRoot,
					    m_fButtonSize,
					    wndArgs.window->getText(),
					    m_pMapPack->getMapInfo()->getName());
    break;
  case EB_MAP_FILENAME:
    m_pEditValueWindow = new CEditBoxString(id,
					    m_pRoot,
					    m_fButtonSize,
					    wndArgs.window->getText(),
					    m_pMapPack->getMapInfo()->getFileName());
    break;
  case EB_ID:
    if (m_pSelectedEntity) {
      m_pEditValueWindow = new CEditBoxString(id,
					      m_pRoot,
					      m_fButtonSize,
					      wndArgs.window->getText(),
					      m_pSelectedEntity->getID());
    }
    break;
  }
  return true;
}
float tesst = 10;
bool CMapEditor::onEditFloat(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  switch (id) {
  case EB_HITPOINTS:
    m_pEditValueWindow = new CEditBoxFloat(id,
					   m_pRoot,
					   m_fButtonSize,
					   wndArgs.window->getText(),
					   dynamic_cast<CHitableObject*>(m_pSelectedEntity)
					   ->getMaximumHitpoints());
    break;
  case EB_DAMAGE:
    m_pEditValueWindow = new CEditBoxFloat(id,
					   m_pRoot,
					   m_fButtonSize,
					   wndArgs.window->getText(),
					   tesst);
    break;
  case EB_SWITCH_ACTIVATION_TIME:
    m_pEditValueWindow = new CEditBoxFloat(id,
					   m_pRoot,
					   m_fButtonSize,
					   wndArgs.window->getText(),
					   dynamic_cast<CSwitch*>(m_pSelectedEntity)
					   ->getActivationTime());
    break;
  }
  return true;
}
bool CMapEditor::onEditUIntVector2(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  switch (id) {
  case EB_MAP_SIZE:
    m_pEditValueWindow = new CEditBoxUIntVector2(id,
						 m_pRoot,
						 m_fButtonSize,
						 wndArgs.window->getText(),
						 m_uiMapSizeX,
						 m_uiMapSizeY);
    m_pEditValueWindow->setListener(this);
    break;
  case EB_SWITCH_ENTRY_POSITION:
    {
      Listbox *pLB = dynamic_cast<Listbox*>(m_pEditSwitchPane->getChild("List"));
      CSwitch *pSwitch = dynamic_cast<CSwitch*>(m_pSelectedEntity);
      if (pSwitch && pLB->getFirstSelectedItem()) {
	CChangeTileEvent *pTileEvent(static_cast<CChangeTileEvent*>(pLB->getFirstSelectedItem()->getUserData()));
	m_pEditValueWindow = new CEditBoxUIntVector2(id,
						     m_pRoot,
						     m_fButtonSize,
						     wndArgs.window->getText(),
						     pTileEvent->getTilePosX(),
						     pTileEvent->getTilePosY());
      }
    }
    break;
  case EB_LINK_TO:
    {
      Listbox *pLinksList = dynamic_cast<Listbox*>(m_pTabControl->getChild("LinkContentPane")->getChild("ScrollPane")->getChild("List"));
      if (pLinksList->getFirstSelectedItem()) {
	CLink *pLink = static_cast<CLink*>(pLinksList->getFirstSelectedItem()->getUserData());
	m_pEditValueWindow = new CEditBoxUIntVector2(id,
						     m_pRoot,
						     m_fButtonSize,
						     wndArgs.window->getText(),
						     pLink->getFirstX(),
						     pLink->getFirstY());
      }
    }
    break;
  case EB_LINK_FROM:
    {
      Listbox *pLinksList = dynamic_cast<Listbox*>(m_pTabControl->getChild("LinkContentPane")->getChild("ScrollPane")->getChild("List"));
      if (pLinksList->getFirstSelectedItem()) {
	CLink *pLink = static_cast<CLink*>(pLinksList->getFirstSelectedItem()->getUserData());
	m_pEditValueWindow = new CEditBoxUIntVector2(id,
						     m_pRoot,
						     m_fButtonSize,
						     wndArgs.window->getText(),
						     pLink->getSecondX(),
						     pLink->getSecondY());
      }
    }
    break;
  }
  return true;
}
bool CMapEditor::onEditBoolChanged(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  ToggleButton *pBut = dynamic_cast<ToggleButton*>(wndArgs.window);
  switch (id) {
  case EB_JUMPING:
    dynamic_cast<CEnemy*>(m_pSelectedEntity)->setMayJump(pBut->isSelected());
    break;
  case EB_SWITCH_AFFECTING_BLOCKS:
    dynamic_cast<CSwitch*>(m_pSelectedEntity)->setFlag(CSwitch::SF_CHANGE_BLOCKS, pBut->isSelected());
    break;
  case EB_SWITCH_DEACTIVABLE:
    dynamic_cast<CSwitch*>(m_pSelectedEntity)->setFlag(CSwitch::SF_DEACTIVATABLE, pBut->isSelected());
    break;
  case EB_SWITCH_TIMED:
    dynamic_cast<CSwitch*>(m_pSelectedEntity)->setFlag(CSwitch::SF_TIMED, pBut->isSelected());
    break;
  }
  return true;
}
bool CMapEditor::onEditTileType(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  switch (id) {
  case EB_SWITCH_ENTRY_TILE_TYPE:
    {
      Listbox *pLB = dynamic_cast<Listbox*>(m_pEditSwitchPane->getChild("List"));
      CSwitch *pSwitch = dynamic_cast<CSwitch*>(m_pSelectedEntity);
      if (pSwitch && pLB->getFirstSelectedItem()) {
	CChangeTileEvent *pTileEvent(static_cast<CChangeTileEvent*>(pLB->getFirstSelectedItem()->getUserData()));
	m_pEditValueWindow = new CEditBoxTileType(id,
						  m_pRoot,
						  m_fButtonSize,
						  wndArgs.window->getText(),
						  pTileEvent->getTileType());
      }
    }
    break;
  }
  return true;
}
bool CMapEditor::onEditCustom(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  int id = PropertyHelper<int>::fromString(wndArgs.window->getName());
  switch (id) {
  case EB_MAP_EXIT:
    m_pEditValueWindow = new CEditBoxExit(id,
					  m_pRoot,
					  m_fButtonSize,
					  wndArgs.window->getText(),
					  m_pMap->getExit());
    m_pEditValueWindow->setListener(this);
    break;
  }
  return true;
}
bool CMapEditor::onDeleteSwitchEntry(const CEGUI::EventArgs &args) {
  Listbox *pLB = dynamic_cast<Listbox*>(m_pEditSwitchPane->getChild("List"));
  ListboxItem *pLBI = pLB->getFirstSelectedItem();
  if (pLBI) {
    m_pSelectedEntity->destroyEvent(static_cast<CChangeTileEvent*>(pLBI->getUserData()));
    pLB->removeItem(pLBI); // auto delete is set
  }
  return true;
}
bool CMapEditor::onAddSwitchEntry(const CEGUI::EventArgs &args) {
  Listbox *pLB = dynamic_cast<Listbox*>(m_pEditSwitchPane->getChild("List"));
  CSwitch *pSwitch = dynamic_cast<CSwitch*>(m_pSelectedEntity);
  CChangeTileEvent *pEvent = new CChangeTileEvent(*m_pMap, *pSwitch);
  pSwitch->addEvent(pEvent);
  pLB->addItem(createSwitchEntry(*pEvent));
  return true;
}
bool CMapEditor::onDeleteLink(const CEGUI::EventArgs &args) {
  Listbox *pLinksList = dynamic_cast<Listbox*>(m_pTabControl->getChild("LinkContentPane")->getChild("ScrollPane")->getChild("List"));
  if (pLinksList->getFirstSelectedItem()) {
    delete static_cast<CLink*>(pLinksList->getFirstSelectedItem()->getUserData());
    pLinksList->removeItem(pLinksList->getFirstSelectedItem());
  }
  return true;
}
bool CMapEditor::onAddLink(const CEGUI::EventArgs &args) {
  CLink *pLink = new CLink(*m_pMap, m_pMap->getLinksEntity());
  Listbox *pLinksList = dynamic_cast<Listbox*>(m_pTabControl->getChild("LinkContentPane")->getChild("ScrollPane")->getChild("List"));
  pLinksList->addItem(createLinkEntry(*pLink));
  return true;
}
Ogre::Vector2 CMapEditor::snappedPos(const Ogre::Vector2 &vPos) {
  if (!m_bSnapToGrid) {
    return vPos;
  }
  float fGridSize = 0.5;
  Ogre::Vector2 v(vPos / fGridSize);
  v.x = floor(v.x + 0.5) * fGridSize;
  v.y = floor(v.y + 0.5) * fGridSize;

  return v;
}
void CMapEditor::selectedEntity(CEntity *pEntity) {
  if (m_pSelectedEntity) {
    // check for updates
    if (dynamic_cast<CHitableObject*>(m_pSelectedEntity)) {
      dynamic_cast<CHitableObject*>(m_pSelectedEntity)->
	setHitpoints(dynamic_cast<CHitableObject*>(m_pSelectedEntity)->getMaximumHitpoints());
    }
  }
  m_pSelectedEntity = pEntity;

  m_pEditSprite->setVisible(false);
  m_pEditSwitchPane->setVisible(false);

  if (m_pSelectedEntity) {
    m_pEditSprite->setVisible(true);
    
    m_pEditSprite->
      getChild(PropertyHelper<int>::toString(EB_HITPOINTS))->
      setVisible(dynamic_cast<CHitableObject*>(m_pSelectedEntity));

    CEnemy *pEnemy = dynamic_cast<CEnemy*>(m_pSelectedEntity);
    m_pEditSprite->
      getChild(PropertyHelper<int>::toString(EB_JUMPING))->
      setVisible(pEnemy);
    if (pEnemy) {
      dynamic_cast<ToggleButton*>(m_pEditSprite->
				  getChild(PropertyHelper<int>::toString(EB_JUMPING)))
	->setSelected(pEnemy->mayJump());
    }

    CSwitch *pSwitch = dynamic_cast<CSwitch*>(m_pSelectedEntity);
    m_pEditSwitchPane->setVisible(pSwitch);
    if (pSwitch) {
      dynamic_cast<ToggleButton*>(m_pEditSwitchPane->
				  getChild(PropertyHelper<int>::toString(EB_SWITCH_AFFECTING_BLOCKS)))
	->setSelected(pSwitch->isFlagSet(CSwitch::SF_CHANGE_BLOCKS));

      dynamic_cast<ToggleButton*>(m_pEditSwitchPane->
				  getChild(PropertyHelper<int>::toString(EB_SWITCH_DEACTIVABLE)))
	->setSelected(pSwitch->isFlagSet(CSwitch::SF_DEACTIVATABLE));

      dynamic_cast<ToggleButton*>(m_pEditSwitchPane->
				  getChild(PropertyHelper<int>::toString(EB_SWITCH_TIMED)))
	->setSelected(pSwitch->isFlagSet(CSwitch::SF_TIMED));

      Listbox *pLB = dynamic_cast<Listbox*>(m_pEditSwitchPane->getChild("List"));
      while (pLB->getItemCount() > 0) {
	pLB->removeItem(pLB->getListboxItemFromIndex(0));
      }
      for (const CEvent *pEvent : pSwitch->getEvents()) {
	if (pEvent->getType() == CEvent::EVENT_CHANGE_TILE) {
	  pLB->addItem(createSwitchEntry(*dynamic_cast<const CChangeTileEvent*>(pEvent)));
	}
      }
      
    }
  }
}
CEGUI::ListboxTextItem *CMapEditor::createLinkEntry(const CLink &link) {
  ListboxTextItem *pItem =
    new ListboxTextItem("link");
  pItem->setSelectionColours(Colour(0.0,0.0,0.5,1));
  pItem->setSelectionBrushImage("OgreTrayImages/GenericBrush");
  pItem->setUserData(const_cast<CLink*>(&link));
  return pItem;
}
CEGUI::ListboxTextItem *CMapEditor::createSwitchEntry(const CChangeTileEvent &event) {
  ListboxTextItem *pItem =
    new ListboxTextItem(event.toString().c_str());
  pItem->setSelectionColours(Colour(0.0,0.0,0.5,1));
  pItem->setSelectionBrushImage("OgreTrayImages/GenericBrush");
  pItem->setUserData(&const_cast<CChangeTileEvent&>(event));
  return pItem;
}
void CMapEditor::onEditBoxAccept(CEditBoxBase *pBase) {
  switch (pBase->getID()) {
  case EB_TILE_ENDANGERED:
    {
      CEditBoxTileType *pTT = dynamic_cast<CEditBoxTileType*>(pBase);
      pTT->getTile()->setEndangeredTileType(pTT->getTile()->getTileType());
    }
    break;
  case EB_MAP_SIZE:
    m_pMap->resizeTiles(m_uiMapSizeX, m_uiMapSizeY);
    break;
  }
}
void CMapEditor::onEditBoxCancel(CEditBoxBase *pBase) {
}
