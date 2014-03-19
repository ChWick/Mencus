#include "MapEditor.hpp"
#include "TileTypes.hpp"
#include "HUD.hpp"
#include "Tile.hpp"
#include "Map.hpp"

using namespace CEGUI;

template<> CMapEditor *Ogre::Singleton<CMapEditor>::msSingleton = 0;

CMapEditor *CMapEditor::getSingletonPtr() {
  return msSingleton;
}
CMapEditor &CMapEditor::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}
CMapEditor::CMapEditor(Window *pRoot) 
  : m_uiCurrentTile(2),
    m_pRoot(pRoot),
    m_pMap(NULL),
    m_pTabControl(NULL) {
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


  Window *pBrushPane = pTabPane->createChild("OgreTray/TabContentPane", "BrushContentPane");
  //pTabPane->addTab(pBrushPane);
  pBrushPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pBrushPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pBrushPane->setText("Brush");


  
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
  pause(PAUSE_MAP_UPDATE);
  CHUD::getSingleton().hide();
  m_bVisible = true;
  m_pTabControl->setVisible(true);
  m_bPressed = false;
}
void CMapEditor::stop() {
  unpause(PAUSE_MAP_UPDATE);
  CHUD::getSingleton().show();
  m_bVisible = false;
  m_pTabControl->setVisible(false);
}
void CMapEditor::toggle() {
  if (m_bVisible) {stop();}
  else {start();}
}
bool CMapEditor::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_TAB) {
    toggle();
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
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  selectTile(getTypeFromTile(wndArgs.window));

  // return false for dragging
  return false;
}
bool CMapEditor::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  m_bPressed = true;
  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs)));
  int x = static_cast<int>(vMapPos.x);
  int y = static_cast<int>(vMapPos.y);

  if (m_uiCurrentTile < TT_COUNT) {
    delete m_pMap->getTile(x, y);
    m_pMap->getTile(x, y) = new CTile(m_pMap, m_pMap->get2dManager(), Ogre::Vector2(x, y), m_uiCurrentTile);
  }
  return true;
}
bool CMapEditor::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  m_bPressed = false;
  return true;
}
bool CMapEditor::mouseMoved( const OIS::MouseEvent &arg ) {
  if (!m_bPressed) {return true;}

  Ogre::Vector2 vMapPos(m_pMap->mouseToMapPos(Ogre::Vector2(arg.state.X.abs, arg.state.Y.abs)));
  int x = static_cast<int>(vMapPos.x);
  int y = static_cast<int>(vMapPos.y);

  if (m_uiCurrentTile < TT_COUNT) {
    delete m_pMap->getTile(x, y);
    m_pMap->getTile(x, y) = new CTile(m_pMap, m_pMap->get2dManager(), Ogre::Vector2(x, y), m_uiCurrentTile);
  }
  return true;
}

