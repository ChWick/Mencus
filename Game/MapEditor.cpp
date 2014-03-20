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
void CMapEditor::init(CMap *pMap) {
  CInputListenerManager::getSingleton().addInputListener(this);
  m_pMap = pMap;

  float fTabSize = 0.3;
  int iTilesCountX = 3;
  float fButtonSize = 100;
  

  TabControl *pTabPane = dynamic_cast<TabControl*>(m_pRoot->createChild("OgreTray/TabControl", "TilesGroup"));
  m_pTabControl = pTabPane;
  pTabPane->setPosition(UVector2(UDim(1 - fTabSize, 0), UDim(0, 0)));
  pTabPane->setSize(USize(UDim(fTabSize, 0), UDim(1, 0)));


  // Brush content pane
  // ==================

  Window *pBrushPane = pTabPane->createChild("OgreTray/TabContentPane", "BrushContentPane");
  //pTabPane->addTab(pBrushPane);
  pBrushPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pBrushPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pBrushPane->setText("Brush");

  ScrollablePane *pBrushScrollPane = dynamic_cast<ScrollablePane*>(pBrushPane->createChild("OgreTray/ScrollablePane", "ScrollPane"));
  pBrushScrollPane->setDragIgnoreZone(fButtonSize * 0.2);
  pBrushScrollPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pBrushScrollPane->setSize(USize(UDim(1, 0), UDim(1, 0)));

  for (int i = 0; i < B_COUNT; i++) {
    RadioButton *pBrushSelection = dynamic_cast<RadioButton*>(pBrushScrollPane->createChild("OgreTray/RadioButton", MAP_EDITOR_BRUSH_LABELS[i]));
    pBrushSelection->setText(MAP_EDITOR_BRUSH_LABELS[i]);
    pBrushSelection->setPosition(UVector2(UDim(0, 0), UDim(0, fButtonSize * i * 0.5)));
    pBrushSelection->setSize(USize(UDim(1, 0), UDim(0, fButtonSize * 0.5)));
    pBrushSelection->setGroupID(812723);
    pBrushSelection->setID(i);
    pBrushSelection->setSelected(false);
    pBrushSelection->subscribeEvent(RadioButton::EventSelectStateChanged,
				    Event::Subscriber(&CMapEditor::onBrushSelectionChanged, this));
  }
  dynamic_cast<RadioButton*>(pBrushScrollPane->getChild(MAP_EDITOR_BRUSH_LABELS[B_PLACE]))->setSelected(true);

  // Tiles content pane
  // ==================
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
}
void CMapEditor::stop() {
  setInputListenerEnabled(false);
  unpause(PAUSE_MAP_UPDATE);
  CHUD::getSingleton().show();
  m_bVisible = false;
  m_pTabControl->setVisible(false);
  m_pMap->resize(Ogre::Vector2(CGame::getSingleton().getRenderWindow()->getWidth(),
			       CGame::getSingleton().getRenderWindow()->getHeight()));
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
  m_bPressed = false;
  return true;
}
bool CMapEditor::mouseMoved( const OIS::MouseEvent &arg ) {
  if (!m_bPressed) {return true;}

  handleBrushMoved(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs));
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
  handleBrushMoved(vPos);
}
void CMapEditor::handleBrushMoved(const Ogre::Vector2 &vPos) {
  switch (m_eSelectedBrush) {
  case B_PLACE:
    placeCurrentTile(vPos);
    break;
  }
}
void CMapEditor::placeCurrentTile(const Ogre::Vector2 &vPos) {
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(vPos));
  if (!m_pMap->isVisible(m_pMap->transformPosition(vMapPos))) {return;}

  int x = static_cast<int>(vMapPos.x);
  int y = static_cast<int>(vMapPos.y);

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
