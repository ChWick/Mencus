#include "MapEditor.hpp"
#include "TileTypes.hpp"
#include "HUD.hpp"
#include "Tile.hpp"
#include "Map.hpp"
#include "Enemy.hpp"
#include "Game.hpp"
#include "Object.hpp"
#include "DebugDrawer.hpp"
#include "Player.hpp"

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
  : m_uiCurrentTile(2),
    m_pRoot(pRoot),
    m_pMap(NULL),
    m_pTabControl(NULL) {
  //init(0);
}
void CMapEditor::init(CMap *pMap, const CMapInfoConstPtr pMapInfo) {
  CInputListenerManager::getSingleton().addInputListener(this);
  m_pMap = pMap;
  m_pMapInfo = std::shared_ptr<CMapInfo>(new CMapInfo(pMapInfo));

  float fTabSize = 0.3;
  int iTilesCountX = 5;
  float fButtonSize = 30;
  

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

  ScrollablePane *pBrushScrollPane = dynamic_cast<ScrollablePane*>(pBrushPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pBrushScrollPane->setDragIgnoreZone(fButtonSize * 0.2);
  pBrushScrollPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pBrushScrollPane->setSize(USize(UDim(1, 0), UDim(1, 0)));

  float fCurrentHeight = 0;
  for (int i = 0; i < B_COUNT; i++) {
    RadioButton *pBrushSelection = dynamic_cast<RadioButton*>(pBrushScrollPane->createChild("OgreTray/RadioButton", MAP_EDITOR_BRUSH_LABELS[i]));
    pBrushSelection->setText(MAP_EDITOR_BRUSH_LABELS[i]);
    pBrushSelection->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
    pBrushSelection->setSize(USize(UDim(1, 0), UDim(0, fButtonSize * 0.5)));
    pBrushSelection->setGroupID(812723);
    pBrushSelection->setID(i);
    pBrushSelection->setSelected(false);
    pBrushSelection->subscribeEvent(RadioButton::EventSelectStateChanged,
				    Event::Subscriber(&CMapEditor::onBrushSelectionChanged, this));
    fCurrentHeight += fButtonSize * 0.5;
  }
  dynamic_cast<RadioButton*>(pBrushScrollPane->getChild(MAP_EDITOR_BRUSH_LABELS[B_PLACE]))->setSelected(true);

  // offset
  fCurrentHeight += 10;

  // create check buttons
  ToggleButton *pSnapToGridButton = dynamic_cast<ToggleButton*>(pBrushScrollPane->createChild("OgreTray/Checkbox", "SnapToGrid"));
  pSnapToGridButton->setPosition(UVector2(UDim(0, 0), UDim(0, fCurrentHeight)));
  pSnapToGridButton->setSize(USize(UDim(1, 0), UDim(0, fButtonSize * 0.5)));
  fCurrentHeight += fButtonSize * 0.5;
  pSnapToGridButton->setText("Snap to grid");
  pSnapToGridButton->setEnabled(false);
  pSnapToGridButton->subscribeEvent(ToggleButton::EventSelectStateChanged,
				    Event::Subscriber(&CMapEditor::onSnapToGridChanged, this));
  pSnapToGridButton->setEnabled(true);

  // offset
  fCurrentHeight += 10;
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
  pTilesTabContentPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pTilesTabContentPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pTilesTabContentPane->setText("Tiles");
  pTilesTabContentPane->setFont("dejavusans12");
  
  
  ScrollablePane *pTilesScrollBar = dynamic_cast<ScrollablePane*>(pTilesTabContentPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pTilesScrollBar->setDragIgnoreZone(fButtonSize * 0.2);
  m_pTilesContainer = pTilesScrollBar;
  pTilesScrollBar->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pTilesScrollBar->setSize(USize(UDim(1, 0), UDim(1, 0)));


  int id = 0;
  for (int y = 0; y < TT_COUNT; y++) {
    for (int x = 0; x < iTilesCountX; x++){
      id++; // tiles count start at 1

      if (id == TT_COUNT) {
	break;
      }

      String tileName = "Tile" + PropertyHelper<int>::toString(id) + ".png";
      ImageManager::getSingleton().addFromImageFile(tileName, tileName, "General");
      Window *pTile = pTilesScrollBar->createChild("OgreTray/StaticImage", tileName);
      //pTile->setProperty("BackgroundEnabled", "False");
      pTile->setProperty("FrameEnabled", "False");
      pTile->setProperty("Image", tileName);
      pTile->setPosition(UVector2(UDim(0, x * fButtonSize * 1.05f), UDim(0, y * fButtonSize * 1.05f)));
      pTile->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
      pTile->
	subscribeEvent(Window::EventMouseButtonUp,
		       Event::Subscriber(&CMapEditor::onTileClicked, this));
      pTile->
	subscribeEvent(Window::EventMouseButtonDown,
		       Event::Subscriber(&CMapEditor::dummyReturnFalse, this));
      pTile->
	subscribeEvent(Window::EventMouseMove,
		       Event::Subscriber(&CMapEditor::dummyReturnFalse, this));

    }
    if (id == TT_COUNT) {
      break;
    }
  }
  
  selectTile(1);
  m_bVisible = false;
  stop();
}
void CMapEditor::exit() {
  CInputListenerManager::getSingleton().removeInputListener(this);
  m_pTabControl->destroy();
}
void CMapEditor::start() {
  setInputListenerEnabled(true);
  pause(PAUSE_MAP_UPDATE);
  CHUD::getSingleton().hide();
  m_bVisible = true;
  m_pTabControl->setVisible(true);
  m_bPressed = false;
  Sizef vSize(m_pTabControl->getPixelSize());
  m_pMap->resize(Ogre::Vector2(CGame::getSingleton().getRenderWindow()->getWidth() - vSize.d_width, vSize.d_height));
  m_pSelectedSprite = NULL;
  
  //reset map, but store camera pos
  Ogre::Vector2 vCamPos(m_pMap->getCameraTargetPos());
  m_pMap->loadMap(m_pMapInfo);
  m_pMap->setCameraPos(vCamPos);
}
void CMapEditor::stop() {
  setInputListenerEnabled(false);
  unpause(PAUSE_MAP_UPDATE);
  CHUD::getSingleton().show();
  m_pTabControl->setVisible(false);
  m_pMap->resize(Ogre::Vector2(CGame::getSingleton().getRenderWindow()->getWidth(),
			       CGame::getSingleton().getRenderWindow()->getHeight()));

  if (m_bVisible) {
    m_pMap->writeToXMLElement(m_pMapInfo->getEmptyRootNode());
    m_bVisible = false;
  }
}
void CMapEditor::render() {
  if (!m_bVisible) {return;}
  if (m_pSelectedSprite) {
    CDebugDrawer::getSingleton().draw(m_pSelectedSprite->getWorldBoundingBox());
  }
}
void CMapEditor::toggle() {
  if (m_bVisible) {stop();}
  else {start();}
}
bool CMapEditor::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_TAB) {
    stop();
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
  getTileFromType(m_uiCurrentTile)->setProperty("FrameEnabled", "False");
  getTileFromType(uiTile)->setProperty("FrameEnabled", "True");
  m_uiCurrentTile = uiTile;
}
bool CMapEditor::onTileClicked(const EventArgs &args) {
  if (m_pTilesContainer->wasDragged()) {return true;}
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  selectTile(getTypeFromTile(wndArgs.window));

  // return false for dragging
  return false;
}
bool CMapEditor::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs)));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return true;}

  // only pressed when clicked on map!
  m_bPressed = true;
  handleBrushPressed(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
  return true;
}
bool CMapEditor::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  if (m_bPressed) {
    handleBrushReleased(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
  }
  m_bPressed = false;
  return true;
}
bool CMapEditor::mouseMoved( const OIS::MouseEvent &arg ) {
  if (!m_bPressed) {return true;}

  handleBrushMoved(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs),
		   Ogre::Vector2(arg.state.X.rel, arg.state.Y.rel));
  return true;
}
bool CMapEditor::selectSprite(const Ogre::Vector2 &vPos) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(vPos));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return false;}

  list<CSprite*> m_lSprites;

  if (m_pMap->getPlayer()->getWorldBoundingBox().contains(vMapPos)) {
    m_lSprites.push_back(m_pMap->getPlayer());
  }
  for (CEnemy *pSprite : m_pMap->getEnemies()) {
    if (pSprite->getWorldBoundingBox().contains(vMapPos)) {
      m_lSprites.push_back(pSprite);
    }
  }
  for (CObject *pSprite : m_pMap->getObjects()) {
    if (pSprite->getWorldBoundingBox().contains(vMapPos)) {
      m_lSprites.push_back(pSprite);
    }
  }

  m_pSelectedSprite = NULL;
  if (m_lSprites.size() > 0) {
    m_pSelectedSprite = m_lSprites.front();
  }
  return true;
}
void CMapEditor::handleBrushPressed(const Ogre::Vector2 &vPos) {
  switch (m_eSelectedBrush) {
  case B_PLACE:
    break;
  case B_EDIT:
  case B_MOVE:
    selectSprite(vPos);
    break;
  }
  handleBrushMoved(vPos, Ogre::Vector2::ZERO);
}
void CMapEditor::handleBrushMoved(const Ogre::Vector2 &vPos, const Ogre::Vector2 &vDelta) {
  switch (m_eSelectedBrush) {
  case B_PLACE:
    placeCurrentTile(vPos);
    break;
  case B_MOVE:
    Ogre::Vector2 vMapPos(m_pMap->mouseToMapSize(vDelta));
    if (m_pSelectedSprite) {
      m_pSelectedSprite->translate(vMapPos);
    }
    else {
      // move map
      m_pMap->translateCamera(-vMapPos);
    }
    break;
  }
}
void CMapEditor::handleBrushReleased(const Ogre::Vector2 &vPos) {
  switch (m_eSelectedBrush) {
  case B_PLACE:
    break;
  case B_MOVE:
    float fGridSize = 0.5;
    if (m_pSelectedSprite && m_bSnapToGrid) {
      Ogre::Vector2 v(m_pSelectedSprite->getPosition() / fGridSize);
      m_pSelectedSprite->setPosition(Ogre::Vector2(floor(v.x + 0.5), floor(v.y + 0.5)) * fGridSize);
    }
    break;
  }
}
void CMapEditor::placeCurrentTile(const Ogre::Vector2 &vPos) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(vPos));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return;}

  int x = static_cast<int>(vMapPos.x);
  int y = static_cast<int>(vMapPos.y);

  if (m_pMap->outOfMap(x, y)) {return;}

  if (m_uiCurrentTile < TT_COUNT) {
    delete m_pMap->getTile(x, y);
    m_pMap->getTile(x, y) = new CTile(m_pMap, m_pMap->get2dManager(), Ogre::Vector2(x, y), m_uiCurrentTile);
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
  tinyxml2::XMLPrinter printer;
  m_pMapInfo->getDocument().Accept(&printer);

  cout << printer.CStr() << endl;
}
