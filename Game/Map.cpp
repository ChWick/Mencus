#include "Map.hpp"
#include "Util.hpp"
#include <tinyxml2.h>
#include "InputListener.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "DebugDrawer.hpp"
#include "DebugText.hpp"
#include "Switch.hpp"
#include "Shot.hpp"
#include "Explosion.hpp"
#include "Enemy.hpp"
#include "Object.hpp"
#include "HUD.hpp"
#include "Game.hpp"
#include "Background.hpp"
#include "XMLHelper.hpp"
#include "Statistics.hpp"
#include "TutorialManager.hpp"
#include "MapEditor.hpp"
#include "Exit.hpp"

using namespace tinyxml2;
using namespace XMLHelper;

const Ogre::Vector2 CTile::DEFAULT_TILE_SIZE(1, 1);
const string CTile::DEFAULT_TILE_TEXTURE_NAME = "tiles/Tile";

const Ogre::Real CAMERA_MAX_MOVE_SPEED(10);

CMap::CMap(Ogre::SceneManager *pSceneManager,
	   CScreenplayListener *pScreenplayListener,
	   SStatistics &statistics,
	   const CMapInfoConstPtr pMapInfo)
  : m_pTutorialManager(new CTutorialManager(*this)), 
    m_p2dManagerMap(NULL),
    m_pBackground(NULL),
    m_vCameraPos(Ogre::Vector2::ZERO),
    m_vCameraTargetPos(Ogre::Vector2::ZERO),
    m_vCameraDebugOffset(Ogre::Vector2::ZERO),
    m_pPlayer(NULL),
    m_pScreenplayListener(pScreenplayListener),
    m_bUpdatePause(false),
    m_bRenderPause(false),
    m_fPlayingTime(0),
  m_Statistics(statistics),
  m_pMapInfo(pMapInfo) {
  resizeTilesPerScreen(Ogre::Vector2(16, 12));
  CGame::getSingleton().showLoadingBar(0, 1);
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Game");
  Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("Game");
#ifdef MAP_EDITOR_ENABLED
  CMapEditor::getSingleton().init(this, pMapInfo);
#endif
  CGame::getSingleton().hideLoadingBar();

  m_p2dManagerMap = new Ogre2dManager();
  m_p2dManagerMap->init(pSceneManager, Ogre::RENDER_QUEUE_BACKGROUND, false);

  m_pDebugSpriteManager =  new Ogre2dManager();
  m_pDebugSpriteManager->init(pSceneManager, Ogre::RENDER_QUEUE_BACKGROUND, true);
  //loadMap("../level/level1/scene3.xml");

  CInputListenerManager::getSingleton().addInputListener(this);

  new CDebugDrawer(this, m_pDebugSpriteManager);

  CHUD::getSingleton().show();

  resize(Ogre::Vector2(CGame::getSingleton().getRenderWindow()->getWidth(),
		       CGame::getSingleton().getRenderWindow()->getHeight()));
}
CMap::~CMap() {
  clearLineNumbers();
#ifdef MAP_EDITOR_ENABLED
  CMapEditor::getSingleton().exit();
#endif
  delete m_pTutorialManager;
  CHUD::getSingleton().hide();
  clearMap();

  if (CDebugDrawer::getSingletonPtr()) {delete CDebugDrawer::getSingletonPtr();}

  CInputListenerManager::getSingleton().removeInputListener(this);
  m_p2dManagerMap->end();
  delete m_p2dManagerMap;

  m_pDebugSpriteManager->end();
  delete m_pDebugSpriteManager;

  Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup("Game");
}
void CMap::clearLineNumbers() {
  for (auto &t : m_vLineNumberX) {
    delete t;
  }
  for (auto &t : m_vLineNumberY) {
    delete t;
  }
  m_vLineNumberX.clear();
  m_vLineNumberY.clear();
}
void CMap::clearMap() {
  m_vCameraRestrictions.clear();
  m_lLinks.clear();
  m_lShotsToDestroy.clear();
  m_lExplosionsToDestroy.clear();
  m_lEnemiesToDestroy.clear();
  m_lObjectsToDestroy.clear();

  if (m_pBackground) {
    delete m_pBackground;
    m_pBackground = NULL;
  }

  while (m_lEnemies.size() > 0) {
    delete m_lEnemies.front();
    m_lEnemies.pop_front();
  }
  while (m_lExplosions.size() > 0) {
    delete m_lExplosions.front();
    m_lExplosions.pop_front();
  }
  while (m_lShots.size() > 0) {
    delete m_lShots.front();
    m_lShots.pop_front();
  }
  while (m_lSwitches.size() > 0) {
    delete m_lSwitches.front();
    m_lSwitches.pop_front();
  }
  while (m_lEnemies.size() > 0) {
    delete m_lEnemies.front();
    m_lEnemies.pop_front();
  }
  while (m_lObjects.size() > 0) {
    delete m_lObjects.front();
    m_lObjects.pop_front();
  }

  if (m_pPlayer) {delete m_pPlayer; m_pPlayer = NULL;}

  for (auto pTile : m_gridTiles) {
    delete pTile;
  }
  m_gridTiles.clear();
}
void CMap::resize(const Ogre::Vector2 &vSize, const Ogre::Vector2 &vOrigin) {
  m_vMapSize = vSize;
  Ogre::Vector2 vScreenSize(CGame::getSingleton().getRenderWindow()->getWidth(),
			    CGame::getSingleton().getRenderWindow()->getHeight());
  Ogre::Vector2 vScale(m_vMapSize / vScreenSize);
  m_vTransformedStartPos = vOrigin / vScreenSize - Ogre::Vector2(1, 1);
  m_vTransformedEndPos = m_vTransformedStartPos + 2 * vScale;
}
void CMap::resizeTiles(unsigned int uiSizeX, unsigned int uiSizeY) {
  grid2d<CTile*> newGrid(uiSizeX, uiSizeY, NULL);

  unsigned int uiCopySizeX = std::min<unsigned int>(uiSizeX, m_gridTiles.getSizeX());
  unsigned int uiCopySizeY = std::min<unsigned int>(uiSizeY, m_gridTiles.getSizeY());
  // copy
  for (unsigned int x = 0; x < uiCopySizeX; x++) {
    for (unsigned int y = 0; y < uiCopySizeY; y++) {
      newGrid(x, y) = m_gridTiles(x, y);
    }
  }
  // create new
  for (unsigned int x = uiCopySizeX; x < uiSizeX; x++) {
    for (unsigned int y = uiCopySizeY; y < uiSizeY; y++) {
      newGrid(x, y) = new CTile(this, this->get2dManager(), Ogre::Vector2(x, y), TT_WALL_FRONT);
    }
  }
  for (unsigned int x = uiCopySizeX; x < uiSizeX; x++) {
    for (unsigned int y = 0; y < m_gridTiles.getSizeY(); y++) {
      newGrid(x, y) = new CTile(this, this->get2dManager(), Ogre::Vector2(x, y), TT_WALL_FRONT);
    }
  }
  for (unsigned int x = 0; x < m_gridTiles.getSizeX(); x++) {
    for (unsigned int y = uiCopySizeY; y < uiSizeY; y++) {
      newGrid(x, y) = new CTile(this, this->get2dManager(), Ogre::Vector2(x, y), TT_WALL_FRONT);
    }
  }
  // delete old
  for (unsigned int x = uiCopySizeX; x < m_gridTiles.getSizeX(); x++) {
    for (unsigned int y = uiCopySizeY; y < m_gridTiles.getSizeY(); y++) {
      delete m_gridTiles(x, y);
    }
  }
  for (unsigned int x = uiCopySizeX; x < m_gridTiles.getSizeX(); x++) {
    for (unsigned int y = 0; y < uiSizeY; y++) {
      delete m_gridTiles(x, y);
    }
  }
  for (unsigned int x = 0; x < uiSizeX; x++) {
    for (unsigned int y = uiCopySizeY; y < m_gridTiles.getSizeY(); y++) {
      delete m_gridTiles(x, y);
    }
  }

  m_gridTiles = newGrid;
}
void CMap::resizeTilesPerScreen(const Ogre::Vector2 &tps) {
  m_vTilesPerScreen = tps;
  m_fScreenRatio = m_vTilesPerScreen.y / m_vTilesPerScreen.x;
  
  for (auto &t : m_vLineNumberX) {
    delete t;
  }
  for (auto &t : m_vLineNumberY) {
    delete t;
  }

  m_vLineNumberX.resize(static_cast<int>(m_vTilesPerScreen.x + 1), NULL);
  m_vLineNumberY.resize(static_cast<int>(m_vTilesPerScreen.y + 1), NULL);
  for (auto &t : m_vLineNumberX) {
    t = new CDebugText();
    t->setText("blabla");
  }
  for (auto &t : m_vLineNumberY) {
    t = new CDebugText();
    t->setText("blubb");
  }
}
void CMap::loadMap(const CMapInfoConstPtr pMapInfo) {
  clearMap();

  const XMLElement *pRoot = pMapInfo->getDocument().FirstChildElement("map");
  readFromXMLElement(pRoot);

  prepareMap();
}
void CMap::loadMap(const string &sFilename, const string &sResourceGroup) {
  clearMap();

  Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource(sFilename, sResourceGroup);
  tinyxml2::XMLDocument doc; // namespace required for windoof (conflicts)
  doc.Parse( pStream->getAsString().c_str() );
  pStream->close();
  pStream.setNull();
  if (doc.Error()) {
    throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, sFilename + " not found.", __FILE__);
  }

  XMLElement *pRoot = doc.FirstChildElement("map");
  readFromXMLElement(pRoot);

  prepareMap();
}
void CMap::prepareMap() {
  // Initialise everything
  for (auto pSwitch : m_lSwitches) {
    pSwitch->initialize(this);
  }

  for (CTile *pTile : m_gridTiles) {
    if (pTile->getEndangeredTileType() != TT_NONE) {
      // to set the endangered flag
      pTile->setEndangeredTileType(pTile->getEndangeredTileType());
    }
  }
  if (m_pPlayer) {
    updateCameraPos(0);
    m_vCameraPos = m_vCameraTargetPos;
  }
  setLineNumbersVisible(false);
}
void CMap::setLineNumbersVisible(bool bVisible) {
  for (auto &t : m_vLineNumberX) {
    t->setVisible(bVisible);
  }
  for (auto &t : m_vLineNumberY) {
    t->setVisible(bVisible);
  }
}
Ogre::Real CMap::hitsTile(ECollisionCheckDirections eCollisionCheckDirection, unsigned uiTileMask, const CBoundingBox2d &bb, CTile **ppTile) const {
  if (outOfMap(bb)) { return 0; } // check if out of map

  // loop trough tiles that could be hit
  int startX = static_cast<int>(bb.getPosition().x + ((eCollisionCheckDirection == CCD_RIGHT) ? bb.getSize().x : 0)); // round down
  int startY = static_cast<int>(bb.getPosition().y + ((eCollisionCheckDirection == CCD_TOP) ? bb.getSize().y : 0)); // round down

  // bonds
  startX = std::max(startX, 0);
  startY = std::max(startY, 0);
  startX = std::min<int>(startX, m_gridTiles.getSizeX() - 1);
  startY = std::min<int>(startY, m_gridTiles.getSizeY() - 1);


  if (eCollisionCheckDirection == CCD_TOP || eCollisionCheckDirection == CCD_BOTTOM) {
    int endX = static_cast<int>(bb.getPosition().x + bb.getSize().x + 0.99); // round up
    endX = std::min<int>(endX, m_gridTiles.getSizeX());

    for (int x = startX; x < endX; ++x) {
      if ((m_gridTiles(x, startY)->getTileFlags() & uiTileMask) == uiTileMask) {
#ifdef DEBUG_COLLISION_WITH_TILES
        CDebugDrawer::getSingleton().draw(m_gridTiles(x, startY));
#endif
        if (ppTile) {
          *ppTile = m_gridTiles(x, startY);
        }
        return bb.getPosition().y - startY - ((eCollisionCheckDirection == CCD_BOTTOM) ? 1 : -bb.getSize().y); // Collision, so penetration
      }
    }
  }
  else {
    int endY = static_cast<int>(bb.getPosition().y + bb.getSize().y + 0.99); // round up
    endY = std::min<int>(endY, m_gridTiles.getSizeY());

    for (int y = startY; y < endY; ++y) {
      if ((m_gridTiles(startX, y)->getTileFlags() & uiTileMask) == uiTileMask) {
#ifdef DEBUG_COLLISION_WITH_TILES
        CDebugDrawer::getSingleton().draw(m_gridTiles(startX, y));
#endif
        if (ppTile) {
          *ppTile = m_gridTiles(startX, y);
        }
        return bb.getPosition().x - startX - ((eCollisionCheckDirection == CCD_LEFT) ? 1 : -bb.getSize().x); // Collision, so penetration
      }
    }
  }

    if (ppTile) {
	    *ppTile = NULL;
	  }

  return 0;			// No collision
}
unsigned int CMap::hitsTile(unsigned int uiTileMask, const CBoundingBox2d &bb, CBoundingBox2d *pbbOverlap, CTile **ppTile) const {
  size_t minx = static_cast<size_t>(bb.getPosition().x);
  size_t miny = static_cast<size_t>(bb.getPosition().y);
  size_t maxx = static_cast<size_t>(bb.getPosition().x + bb.getSize().x);
  size_t maxy = static_cast<size_t>(bb.getPosition().y + bb.getSize().y);

  minx = max(minx, static_cast<size_t>(0));
  miny = max(miny, static_cast<size_t>(0));
  maxx = min(maxx, m_gridTiles.getSizeX() - 1);
  maxy = min(maxy, m_gridTiles.getSizeY() - 1);

  for (size_t x = minx; x <= maxx; ++x) {
    for (size_t y = miny; y <= maxy; ++y) {
#ifdef DEBUG_COLLISION_WITH_TILES
      CDebugDrawer::getSingleton().draw(m_gridTiles(x, y));
#endif
      if ((m_gridTiles(x, y)->getTileFlags() & uiTileMask) == uiTileMask) {
	unsigned int uiCCD = bb.collidesWith(m_gridTiles(x, y)->getWorldBoundingBox(), pbbOverlap);
	if (uiCCD != CCD_NONE) {
	  if (ppTile) {
	    *ppTile = m_gridTiles(x, y);
	  }
	  return uiCCD;
	}
      }
    }
  }
  return CCD_NONE;
}
bool CMap::outOfMap(const CBoundingBox2d &bb, ECollisionCheckDirections eCollisionCheckDirection) const {
  if (eCollisionCheckDirection & CCD_LEFT)
    if (bb.getPosition().x + bb.getSize().x < 0) return true;

  if (eCollisionCheckDirection & CCD_BOTTOM)
    if (bb.getPosition().y + bb.getSize().y < 0) return true;

  if (eCollisionCheckDirection & CCD_RIGHT)
    if (bb.getPosition().x > m_gridTiles.getSizeX()) return true;

  if (eCollisionCheckDirection & CCD_TOP)
    if (bb.getPosition().y > m_gridTiles.getSizeY()) return true;

  return false;
}
bool CMap::collidesWithMapMargin(const CBoundingBox2d &bb) const {
  if (bb.getPosition().x < 0) return true;
  if (bb.getPosition().y < 0) return true;
  if (bb.getPosition().x + bb.getSize().x > m_gridTiles.getSizeX()) return true;
  if (bb.getPosition().y + bb.getSize().y > m_gridTiles.getSizeY()) return true;

  return false;
}
void CMap::activateSwitchOnHit(const CBoundingBox2d &bb) {
  for (auto pSwitch : m_lSwitches) {
    if (pSwitch->getWorldBoundingBox().collidesWith(bb)) {
      pSwitch->activate(this);
      if (pSwitch->doesChangeBlocks()) {
	// TODO: Change blocks
      }
    }
  }
}
void CMap::createExplosion(const Ogre::Vector2 &vCenter, Ogre::Real r) {
  // create explosion
  addExplosion(new CExplosion(this, vCenter, CExplosion::ET_BOMB));

  // check enemies
  for (auto pEnemy : m_lEnemies) {
    if (vCenter.squaredDistance(pEnemy->getCenter()) < r * r) {
      pEnemy->takeDamage(CShot::SHOT_DAMAGE[CExplosion::ET_BOMB]);
    }
  }
  if (vCenter.squaredDistance(m_pPlayer->getCenter()) < r * r) {
    m_pPlayer->takeDamage(CShot::SHOT_DAMAGE[CExplosion::ET_BOMB]);
  }
  // check endangered tiles
  int minx = static_cast<int>(vCenter.x - r);
  int maxx = static_cast<int>(vCenter.x + r);
  int miny = static_cast<int>(vCenter.y - r);
  int maxy = static_cast<int>(vCenter.y + r);

  minx = max(minx, 0);
  maxx = min(maxx, static_cast<int>(m_gridTiles.getSizeX()));
  miny = max(miny, 0);
  maxy = min(maxy, static_cast<int>(m_gridTiles.getSizeY()));

  for (int cx = minx; cx < maxx; cx++) {
    for (int cy = miny; cy < maxy; cy++) {
      if (m_gridTiles(cx, cy)->getCenter().squaredDistance(vCenter) < r * r) {
        explodeTile(cx, cy, true);
      }
    }
  }
}
void CMap::explodeTile(size_t x, size_t y, bool bExplodeNeighbours) {
  CTile* &tile = m_gridTiles(x, y);
  if ((tile->getTileFlags() & CTile::TF_ENDANGERED) == 0) {
    return;			// This tile is not endangered, cancel
  }
  TileType tt = tile->getEndangeredTileType();
  delete tile;
  tile = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y), tt);

  // remove all scratches that collides with this tile
  for (CObject* pObject : m_lObjects) {
    if (pObject->getWorldBoundingBox().collidesWith(tile->getWorldBoundingBox()) != CCD_NONE) {
      destroyObject(pObject);
    }
  }

  if (bExplodeNeighbours) {
    // explode neighbours only left top bottom right
    if (x - 1 >= 0) {explodeTile(x - 1, y, false);}
    if (x + 1 < m_gridTiles.getSizeX()) {explodeTile(x + 1, y, false);}
    if (y - 1 >= 0) {explodeTile(x, y - 1, false);}
    if (y + 1 < m_gridTiles.getSizeY()) {explodeTile(x, y + 1, false);}
  }
}
bool CMap::findLink(const CBoundingBox2d &bb, Ogre::Vector2 &vFromPos, Ogre::Vector2 &vToPos) const {
  for (const auto &link : m_lLinks) {
    if (link.isActivated() == false) {
      continue;
    }
    if (((m_gridTiles(link.getFirstX(), link.getFirstY())->getTileFlags() & (CTile::TF_DOOR1 | CTile::TF_DOOR2 | CTile::TF_DOOR3)) == 0) ||
	((m_gridTiles(link.getSecondX(), link.getSecondY())->getTileFlags() & (CTile::TF_DOOR1 | CTile::TF_DOOR2 | CTile::TF_DOOR3)) == 0)) {
      continue;
    }

    if (link.getLinkDirection() & CLink::LD_FIRST_TO_SECOND) {
      if (m_gridTiles(link.getFirstX(), link.getFirstY())->getWorldBoundingBox().collidesWith(bb) != CCD_NONE) {
        vFromPos = Ogre::Vector2(link.getFirstX(), link.getFirstY());
        vToPos   = Ogre::Vector2(link.getSecondX(), link.getSecondY());
        return true;
      }
    }

    if (link.getLinkDirection() & CLink::LD_SECOND_TO_FIRST) {
      if (m_gridTiles(link.getSecondX(), link.getSecondY())->getWorldBoundingBox().collidesWith(bb) != CCD_NONE) {
        vFromPos = Ogre::Vector2(link.getSecondX(), link.getSecondY());
        vToPos   = Ogre::Vector2(link.getFirstX(), link.getFirstY());
        return true;
      }
    }
  }

  return false;
}
ECollisionCheckDirections CMap::playerVisibleFromPoint(const Ogre::Vector2 &vFromPos, Ogre::Real fMaxDistance, ECollisionCheckDirections eCollisionCheckDirection) const {
  if (vFromPos.squaredDistance(m_pPlayer->getCenter()) > fMaxDistance * fMaxDistance) {return CCD_NONE;}

  if ((eCollisionCheckDirection & CCD_HORIZONTAL) > 0) {
    if (vFromPos.y > m_pPlayer->getPosition().y + m_pPlayer->getSize().y) {
      return CCD_NONE;
    }
    if (vFromPos.y < m_pPlayer->getPosition().y) {
      return CCD_NONE;
    }
  }
  bool bCollFound = false;
  if ((eCollisionCheckDirection & CCD_RIGHT) == CCD_RIGHT) {
    if (m_pPlayer->getCenter().x >= vFromPos.x) {
      for (int x = vFromPos.x; x < m_pPlayer->getCenter().x; x++) {
        if (m_gridTiles(x, static_cast<int>(vFromPos.y))->getTileFlags() & CTile::TF_UNPASSABLE) {
          bCollFound = true;
        }
      }
      if (!bCollFound) return CCD_RIGHT;
    }
  }
  bCollFound = false;
  if ((eCollisionCheckDirection & CCD_LEFT) == CCD_LEFT) {
    if (m_pPlayer->getCenter().x <= vFromPos.x) {
      for (int x = vFromPos.x; x > m_pPlayer->getCenter().x; x--) {
        if (m_gridTiles(x, static_cast<int>(vFromPos.y))->getTileFlags() & CTile::TF_UNPASSABLE) {
          bCollFound = true;
        }
      }
      if (!bCollFound) return CCD_LEFT;
    }
  }
  return CCD_NONE;
}
void CMap::unlock(unsigned int x, unsigned int y) {
  TileType id = m_gridTiles(x, y)->getTileType();
  if (id == 49) {
    delete m_gridTiles(x, y);
    delete m_gridTiles(x, y + 1);

    m_gridTiles(x, y) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y), 2);
    m_gridTiles(x, y + 1) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y + 1), 51);
  }
  else if (id == 50) {
    delete m_gridTiles(x, y);
    delete m_gridTiles(x, y - 1);

    m_gridTiles(x, y) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y), 51);
    m_gridTiles(x, y - 1) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y - 1), 2);
  }
  else if (id == TT_DOOR_WOOD_BOT_CLOSED) {
    delete m_gridTiles(x, y);
    delete m_gridTiles(x, y + 1);

    m_gridTiles(x, y) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y), TT_DOOR_WOOD_BOT_OPEN);
    m_gridTiles(x, y + 1) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y + 1), TT_DOOR_WOOD_TOP_OPEN);
  }
  else if (id == TT_DOOR_WOOD_TOP_CLOSED) {
    delete m_gridTiles(x, y);
    delete m_gridTiles(x, y - 1);

    m_gridTiles(x, y) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y), TT_DOOR_WOOD_TOP_OPEN);
    m_gridTiles(x, y - 1) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y - 1), TT_DOOR_WOOD_BOT_OPEN);
  }
}
void CMap::swapBoxes() {
  for (int x = m_gridTiles.getSizeX() - 1; x >= 0; --x) {
    for (int y = m_gridTiles.getSizeY() - 1; y >= 0; --y) {
      if (m_gridTiles(x, y)->getTileFlags() & CTile::TF_CHANGEBLOCK) {
        TileType ttEndangered = m_gridTiles(x, y)->getEndangeredTileType();
        if (m_gridTiles(x, y)->getTileType() == 67) {
          delete m_gridTiles(x, y);
          m_gridTiles(x, y) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y), 68);
        }
        else if (m_gridTiles(x, y)->getTileType() == 68) {
          delete m_gridTiles(x, y);
          m_gridTiles(x, y) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(x, y), 67);
        }
        m_gridTiles(x, y)->setEndangeredTileType(ttEndangered);
      }
    }
  }
}
bool CMap::keyPressed( const OIS::KeyEvent &arg ) {
  if (!m_bUpdatePause) {
    if (arg.key == OIS::KC_H) {
      m_vCameraDebugOffset.x -= 1;
    }
    else if (arg.key == OIS::KC_K) {
      m_vCameraDebugOffset.x += 1;
    }
    else if (arg.key == OIS::KC_U) {
      m_vCameraDebugOffset.y += 1;
    }
    else if (arg.key == OIS::KC_J) {
      m_vCameraDebugOffset.y -=1;
    }
    else if (arg.key == OIS::KC_ADD) {
      resizeTilesPerScreen(m_vTilesPerScreen + Ogre::Vector2(m_fScreenRatio, 1));
    }
    else if (arg.key == OIS::KC_SUBTRACT) {
      resizeTilesPerScreen(m_vTilesPerScreen - Ogre::Vector2(m_fScreenRatio, 1));
    }
#ifdef MAP_EDITOR_ENABLED
    else if (arg.key == OIS::KC_TAB || arg.key == OIS::KC_RWIN) {
      CMapEditor::getSingleton().start();
      setLineNumbersVisible(true);
    }
#endif
  }

  return true;
}
bool CMap::keyReleased( const OIS::KeyEvent &arg ) {
  return true;
}
void CMap::mapRenderPauseChanged(bool bPause) {
  m_bRenderPause = bPause;
#ifdef MAP_EDITOR_ENABLED
  CMapEditor::getSingleton().setVisible(!bPause);
#endif
}
void CMap::update(Ogre::Real tpf) {
  if (!m_bUpdatePause) {
    m_fPlayingTime += tpf;
    CHUD::getSingleton().setCurrentTime(m_fPlayingTime);

    m_pTutorialManager->update();

    updateCameraPos(tpf);

    // order of updates exquates drawing order, last one will be on top
    updateBackground(tpf);
    for (auto pTile : m_gridTiles) {
      pTile->update(tpf);
    }
    for (auto pObject : m_lObjects) {
      pObject->update(tpf);
    }
    if (!CPauseManager::getSingleton().isPause(PAUSE_ENEMY_MOVEMENT)) {
      for (auto pEnemy : m_lEnemies) {
        pEnemy->update(tpf);
      }
    }
    for (auto pSwitch : m_lSwitches) {
      pSwitch->update(tpf);
    }
    if (!CPauseManager::getSingleton().isPause(PAUSE_SHOT_MOVEMENT)) {
      for (auto pShot : m_lShots) {
        pShot->update(tpf);
      }
    }

    m_pPlayer->update(tpf);

    for (auto pExplosion : m_lExplosions) {
      pExplosion->update(tpf);
    }
    
    if (m_Exit.isInExit(m_pPlayer, this)) {
      m_pScreenplayListener->playerExitsMap();
    }

#ifdef DEBUG_SHOW_OGRE_TRAY
    CGame::getSingleton().getDetailsPanel()->setParamValue(0, Ogre::StringConverter::toString(m_vCameraPos.x));
    CGame::getSingleton().getDetailsPanel()->setParamValue(1, Ogre::StringConverter::toString(m_vCameraPos.y));
    CGame::getSingleton().getDetailsPanel()->setParamValue(2, Ogre::StringConverter::toString(m_pPlayer->getPosition().x));
    CGame::getSingleton().getDetailsPanel()->setParamValue(3, Ogre::StringConverter::toString(m_pPlayer->getPosition().y));
#endif // DEBUG_SHOW_OGRE_TRAY

    // tidy up
    while (m_lShotsToDestroy.size() > 0) {
      delete m_lShotsToDestroy.front();
      m_lShots.remove(m_lShotsToDestroy.front());
      m_lShotsToDestroy.pop_front();
    }
    while(m_lExplosionsToDestroy.size() > 0) {
      delete m_lExplosionsToDestroy.front();
      m_lExplosions.remove(m_lExplosionsToDestroy.front());
      m_lExplosionsToDestroy.pop_front();
    }
    while (m_lEnemiesToDestroy.size() > 0) {
      delete m_lEnemiesToDestroy.front();
      m_lEnemies.remove(m_lEnemiesToDestroy.front());
      m_lEnemiesToDestroy.pop_front();
    }
    while (m_lObjectsToDestroy.size() > 0) {
      delete m_lObjectsToDestroy.front();
      m_lObjects.remove(m_lObjectsToDestroy.front());
      m_lObjectsToDestroy.pop_front();
    }
  }
  if (!m_bRenderPause) {
  // draw links if debug
#ifdef DEBUG_LINKS
    for (auto &link : m_lLinks) {
      CDebugDrawer::getSingleton().draw(link);
    }
#endif
#ifdef DEBUG_EXIT
    m_Exit.debugDraw();
#endif // DEBUG_EXIT
  }

  m_Statistics.fTime = m_fPlayingTime;
}
void CMap::render(Ogre::Real tpf) {
  if (m_bRenderPause) {
    return;
  }
  // order of updates exquates drawing order, last one will be on top
  renderBackground(tpf);
  //for (auto pTile : m_gridTiles) {
  //  pTile->render(tpf);
  //}
  unsigned int xmin = std::max<int>(0, static_cast<int>(m_vCameraPos.x));
  unsigned int ymin = std::max<int>(0, static_cast<int>(m_vCameraPos.y));
  unsigned int xmax = std::min<unsigned int>(floor(m_vCameraPos.x + m_vTilesPerScreen.x + 1), m_gridTiles.getSizeX());
  unsigned int ymax = std::min<unsigned int>(floor(m_vCameraPos.y + m_vTilesPerScreen.y + 1), m_gridTiles.getSizeY());


  
  for (int x = xmin; x < xmax; x++) {
    for (int y = ymin; y < ymax; y++) {
      m_gridTiles(x, y)->render(tpf);
    }
  }
#ifdef MAP_EDITOR_ENABLED
  if (CMapEditor::getSingleton().isVisible()) {
    // line numbers
    for (int x = 0; x < m_vLineNumberX.size(); x++) {
      m_vLineNumberX[x]->setText(Ogre::StringConverter::toString(x + static_cast<int>(m_vCameraPos.x)));
      m_vLineNumberX[x]->setPos((static_cast<int>(m_vCameraPos.x) - m_vCameraPos.x + x) / m_vTilesPerScreen.x, 0);
    }
    for (int y = 0; y < m_vLineNumberY.size(); y++) {
      m_vLineNumberY[y]->setText(Ogre::StringConverter::toString(static_cast<int>(floor(m_vCameraPos.y + m_vTilesPerScreen.y)) - y));
      m_vLineNumberY[y]->setPos(0, (m_vCameraPos.y - static_cast<int>(floor(m_vCameraPos.y
									    )) + y) / m_vTilesPerScreen.y - 0.05);
    }
    // endangered tiles
    for (int x = xmin; x < xmax; x++) {
      for (int y = ymin; y < ymax; y++) {
	if (m_gridTiles(x, y)->getEndangeredTileType() != TT_NONE) {
	  CDebugDrawer::getSingleton().draw(m_gridTiles(x, y), m_gridTiles(x, y)->getEndangeredTileType(), 0.5);
	}
      }
    }
  }
#endif

  for (auto pObject : m_lObjects) {
    pObject->render(tpf);
  }
  for (auto pEnemy : m_lEnemies) {
    pEnemy->render(tpf);
  }
  for (auto pSwitch : m_lSwitches) {
    pSwitch->render(tpf);
  }
  for (auto pShot : m_lShots) {
    pShot->render(tpf);
  }

  m_pPlayer->render(tpf);

  for (auto pExplosion : m_lExplosions) {
    pExplosion->render(tpf);
  }
}
void CMap::updateBackground(Ogre::Real tpf) {
  if (m_pBackground) {
    m_pBackground->update(tpf);
  }
}
void CMap::renderBackground(Ogre::Real tpf) {
  if (m_pBackground) {
    m_pBackground->render(tpf);
  }
}
void CMap::updateCameraPos(Ogre::Real tpf) {
  Ogre::Vector2 vCenter = m_pPlayer->getPosition() + m_pPlayer->getSize() / 2;
  m_vCameraTargetPos = vCenter - m_vTilesPerScreen/ 2;
  m_vCameraTargetPos.x = max(m_vCameraTargetPos.x, static_cast<Ogre::Real>(0));
  m_vCameraTargetPos.y = max(m_vCameraTargetPos.y, static_cast<Ogre::Real>(0));

  m_vCameraTargetPos.x = min(m_vCameraTargetPos.x, m_gridTiles.getSizeX() - m_vTilesPerScreen.x);
  m_vCameraTargetPos.y = min(m_vCameraTargetPos.y, m_gridTiles.getSizeY() - m_vTilesPerScreen.y);

  for (auto &camRestr : m_vCameraRestrictions) {
    camRestr.update(m_vCameraTargetPos, m_pPlayer->getCenter());
  }
  Ogre::Vector2 vDir(m_vCameraTargetPos - m_vCameraPos);
  m_vCameraPos += vDir * std::min(vDir.normalise(), tpf * CAMERA_MAX_MOVE_SPEED);

  // bounds
  m_vCameraPos.x = max(m_vCameraPos.x, static_cast<Ogre::Real>(0));
  m_vCameraPos.y = max(m_vCameraPos.y, static_cast<Ogre::Real>(0));
  m_vCameraPos.x = min(m_vCameraPos.x, m_gridTiles.getSizeX() - m_vTilesPerScreen.x);
  m_vCameraPos.y = min(m_vCameraPos.y, m_gridTiles.getSizeY() - m_vTilesPerScreen.y);
}
Ogre::Vector2 CMap::mouseToMapPos(const Ogre::Vector2 &vMousePos) const {
  Ogre::RenderWindow *pWnd = CGame::getSingleton().getRenderWindow();

  return m_vCameraPos + m_vTilesPerScreen * (Ogre::Vector2(0, 1) + vMousePos / Ogre::Vector2(pWnd->getWidth(), -static_cast<Ogre::Real>(pWnd->getHeight())));
}
Ogre::Vector2 CMap::mouseToMapSize(const Ogre::Vector2 &vMousePos) const {
  Ogre::RenderWindow *pWnd = CGame::getSingleton().getRenderWindow();

  return m_vTilesPerScreen * vMousePos / Ogre::Vector2(pWnd->getWidth(), -static_cast<Ogre::Real>(pWnd->getHeight()));
}
// SpriteTransformPipeline
Ogre::Vector2 CMap::transformPosition(const Ogre::Vector2 &vPosition) const {
  Ogre::Vector2 vOffset(m_vCameraPos + m_vCameraDebugOffset);
  return ((vPosition - vOffset) / m_vTilesPerScreen * 2 - Ogre::Vector2(1, 1));
}
bool CMap::isVisible(const Ogre::Vector2 &vPosition) const {
  return m_vTransformedStartPos.y <= vPosition.y
    && m_vTransformedStartPos.x <= vPosition.x
    && m_vTransformedEndPos.y >= vPosition.y
    && m_vTransformedEndPos.x >= vPosition.x;
}
bool CMap::isVisible(const Ogre::Vector2 &vStart,
					 const Ogre::Vector2 &vEnd) const {
  return !(vEnd.x < m_vTransformedStartPos.x || vEnd.y < m_vTransformedStartPos.y || vStart.x > m_vTransformedEndPos.x || vStart.y > m_vTransformedEndPos.y);
}
bool CMap::isInMap(unsigned int x, unsigned int y) {
    if (x < 0) {return false;}
    if (y < 0) {return false;}

    if (x >= m_gridTiles.getSizeX()) {return false;}
    if (y >= m_gridTiles.getSizeY()) {return false;}

    return true;
}
void CMap::readRow(const XMLElement *pRow, unsigned int uiRow) {
  std::string tiles = pRow->Attribute("tiles");

  auto vTiles = Ogre::StringUtil::split(tiles, " ");

  if (vTiles.size() != m_gridTiles.getSizeX()) {
    throw Ogre::Exception(0, "Wrong tile count in parsing a row.", __FILE__);
  }

  unsigned int uiColumn = 0;
  for (const auto &tile : vTiles) {
    unsigned int uiTileID = Ogre::StringConverter::parseInt(tile);
    m_gridTiles(uiColumn, uiRow) = new CTile(this, m_p2dManagerMap, Ogre::Vector2(uiColumn, uiRow), uiTileID);
    ++uiColumn;
  }
}
void CMap::readSwitch(const XMLElement *pSwitch) {
  CSwitch *pNewSwitch =
    new CSwitch(this,
		m_p2dManagerMap,
		Ogre::Vector2(pSwitch->FloatAttribute("x"),
			      pSwitch->FloatAttribute("y")),
		pSwitch->IntAttribute("type"),
		BoolAttribute(pSwitch, "affectsBlocks", true),
		EnumAttribute<CSwitch::ESwitchStates>(pSwitch,
						      "state",
						      CSwitch::SS_DEACTIVATED));
  for (const XMLElement *pChange = pSwitch->FirstChildElement(); pChange; pChange = pChange->NextSiblingElement()) {
    if (strcmp(pChange->Value(), "changes") == 0) {
      SSwitchEntry entry;
      entry.uiTileType = pChange->IntAttribute("id");
      entry.uiTilePosX = pChange->IntAttribute("x");
      entry.uiTilePosY = pChange->IntAttribute("y");
      if (pChange->Attribute("oldid")) {
	entry.uiOldTileType = pChange->IntAttribute("oldid");
      }

      pNewSwitch->addEntry(entry);
    }
    else if (strcmp(pChange->Value(), "togglesLink") == 0) {
      STogglesLinkEntry entry;
      entry.sLinkID = pChange->Attribute("id");
      entry.bInitialState = BoolAttribute(pChange, "initial", true);

      pNewSwitch->addEntry(entry);
    }
  }

  m_lSwitches.push_back(pNewSwitch);
}
void CMap::readEndangeredTiles(const XMLElement *pTile) {
  TileType tt = pTile->IntAttribute("targetTile");
  size_t x = pTile->IntAttribute("x");
  size_t y = pTile->IntAttribute("y");

  m_gridTiles(x, y)->setEndangeredTileType(tt);

  Ogre::LogManager::getSingleton().logMessage("Parsed endangered tile at (" + Ogre::StringConverter::toString(x) + ", " + Ogre::StringConverter::toString(y) + ") with target tile type " + Ogre::StringConverter::toString(tt));
}

void CMap::readLink(const XMLElement *pLink) {
  Ogre::String sID(Ogre::StringUtil::BLANK);
  if (pLink->Attribute("id")) {sID = pLink->Attribute("id");}

  Ogre::String sLinkDirection("both");
  if (pLink->Attribute("direction")) {sLinkDirection = pLink->Attribute("direction");}
  CLink::ELinkDirection eLD = CLink::parseLinkDirection(sLinkDirection);
  
  m_lLinks.push_back(CLink(
                           pLink->IntAttribute("fromx"),
                           pLink->IntAttribute("fromy"),
                           pLink->IntAttribute("tox"),
                           pLink->IntAttribute("toy"),
                           eLD,
                           sID,
			   BoolAttribute(pLink, "activated", true)));
  Ogre::LogManager::getSingleton().logMessage("Parsed: " + m_lLinks.back().toString());
}
void CMap::readExit(const XMLElement *pExitElem) {
  Ogre::String sType = pExitElem->Attribute("type");

  if (sType == "region") {
    CBoundingBox2d bb(Ogre::Vector2(pExitElem->FloatAttribute("posx"),
                                    pExitElem->FloatAttribute("posy")),
                      Ogre::Vector2(pExitElem->FloatAttribute("sizex"),
                                    pExitElem->FloatAttribute("sizey")));
    m_Exit = CExit::newRegion(bb);
  }
  else if (sType == "enemyDeath") {
    m_Exit = CExit::newEnemyDeath(pExitElem->Attribute("id"));
  }
  else {
    throw Ogre::Exception(0, sType + " is not a valid exit type", __FILE__);
  }
}
void CMap::readCamera(const tinyxml2::XMLElement *pCamera) {
  m_vCameraPos = Vector2Attribute(pCamera, "pos");
  for (const XMLElement *pElement = pCamera->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement()) {
    if (std::string(pElement->Value()) == "restriction") {
      if (strcmp(pElement->Attribute("type"), "horizontal") == 0){
        m_vCameraRestrictions.push_back(CCameraRestriction(CCameraRestriction::HORIZONTAL_RESTRICTION, m_vTilesPerScreen, pElement->FloatAttribute("y")));
      } else if (strcmp(pElement->Attribute("type"), "vertical") == 0){
        m_vCameraRestrictions.push_back(CCameraRestriction(CCameraRestriction::VERTICAL_RESTRICTION, m_vTilesPerScreen, pElement->FloatAttribute("x")));
      }
    }
  }
}
CLink *CMap::getLinkById(const Ogre::String &id) {
  for (CLink &link : m_lLinks) {
    if (link.getID() == id) {
      return &link;
    }
  }
  return NULL;
}
CEnemy *CMap::getEnemyById(const Ogre::String &id) {
  for (CEnemy *pEnemy : m_lEnemies) {
    if (pEnemy->getID() == id) {
      return pEnemy;
    }
  }
  return NULL;
}
void CMap::playerWarped() {
  m_vCameraTargetPos = m_pPlayer->getCenter() - m_vTilesPerScreen * 0.5;
  for (auto &camRestr : m_vCameraRestrictions) {
    camRestr.update(m_vCameraTargetPos, m_pPlayer->getCenter());
  }
  m_vCameraPos = m_vCameraTargetPos;
}
void CMap::destroySwich(CSwitch *pSwitch) {
  m_lSwitches.remove(pSwitch);
  delete pSwitch;
}
void CMap::destroyEnemy(CEnemy *pEnemy, bool bLater) {
  for (CShot *pShot : m_lShots) {
    pShot->enemyDestroyed(pEnemy);
  }

  if (bLater) {
    m_lEnemiesToDestroy.push_back(pEnemy);
  }
  else {
    m_lEnemies.remove(pEnemy);
    delete pEnemy;
  }
}
void CMap::destroyObject(CObject *pObject, bool bLater) {
  if (bLater) {
    if (find(m_lObjectsToDestroy.begin(),
	     m_lObjectsToDestroy.end(),
	     pObject) == m_lObjectsToDestroy.end()) {
    m_lObjectsToDestroy.push_back(pObject);
    }
  }
  else {
    m_lObjects.remove(pObject);
    delete pObject;
  }
}

void CMap::writeToXMLElement(tinyxml2::XMLElement *pMapElem) const {
  using namespace tinyxml2;

  XMLDocument &doc = *pMapElem->GetDocument();
  
  if (m_pBackground) {
    pMapElem->SetAttribute("background", m_pBackground->getName().c_str());
  }

  pMapElem->SetAttribute("sizex", static_cast<unsigned>(m_gridTiles.getSizeX()));
  pMapElem->SetAttribute("sizey", static_cast<unsigned>(m_gridTiles.getSizeY()));

  XMLElement *pTiles = doc.NewElement("tiles");
  pMapElem->InsertEndChild(pTiles);
  pTiles->SetAttribute("invert", false);
  for (unsigned int y = 0; y < m_gridTiles.getSizeY(); y++) {
    XMLElement *pRow = doc.NewElement("row");
    pTiles->InsertEndChild(pRow);
    std::stringstream ss;
    for (unsigned int x = 0; x < m_gridTiles.getSizeX(); x++) {
      ss << m_gridTiles(x, y)->getTileType() << " ";
    }
    pRow->SetAttribute("tiles", ss.str().c_str());
  }
  
  XMLElement *pSwitches = doc.NewElement("switches");
  pMapElem->InsertEndChild(pSwitches);
  for (CSwitch *pSwitch : m_lSwitches) {
    XMLElement *pSwitchElem = doc.NewElement("switch");
    pSwitches->InsertEndChild(pSwitchElem);
    pSwitchElem->SetAttribute("x", pSwitch->getPosition().x);
    pSwitchElem->SetAttribute("y", pSwitch->getPosition().y);
    pSwitchElem->SetAttribute("type", pSwitch->getType());
    pSwitchElem->SetAttribute("affectsBlocks", pSwitch->doesChangeBlocks());
    pSwitchElem->SetAttribute("state", pSwitch->getState());
    for (const SSwitchEntry &entry : pSwitch->getEntries()) {
      XMLElement *pChange = doc.NewElement("changes");
      pSwitchElem->InsertEndChild(pChange);
      pChange->SetAttribute("id", entry.uiTileType);
      pChange->SetAttribute("oldid", entry.uiOldTileType);
      pChange->SetAttribute("x", entry.uiTilePosX);
      pChange->SetAttribute("y", entry.uiTilePosY);
    }
    for (const STogglesLinkEntry &entry : pSwitch->getLinkEntries()) {
      XMLElement *pChange = doc.NewElement("togglesLink");
      pSwitchElem->InsertEndChild(pChange);
      pChange->SetAttribute("id", entry.sLinkID.c_str());
      pChange->SetAttribute("initial", entry.bInitialState);
    }
  }

  XMLElement *pEndangeredTiles = doc.NewElement("endangeredTiles");
  pMapElem->InsertEndChild(pEndangeredTiles);
  for (unsigned int y = 0; y < m_gridTiles.getSizeY(); y++) {
    for (unsigned int x = 0; x < m_gridTiles.getSizeX(); x++) {
      if (m_gridTiles(x,y)->getTileFlags() & CTile::TF_ENDANGERED) {
	XMLElement *pEndangeredTile = doc.NewElement("tile");
	pEndangeredTiles->InsertEndChild(pEndangeredTile);
	pEndangeredTile->SetAttribute("targetTile", m_gridTiles(x, y)->getEndangeredTileType());
	pEndangeredTile->SetAttribute("x", x);
	pEndangeredTile->SetAttribute("y", y);
      }
    }
  }

  XMLElement *pLinks = doc.NewElement("links");
  pMapElem->InsertEndChild(pLinks);
  for (const CLink &link : m_lLinks) {
    XMLElement *pLink = doc.NewElement("link");
    pLinks->InsertEndChild(pLink);
    pLink->SetAttribute("id", link.getID().c_str());
    pLink->SetAttribute("fromx", link.getFirstX());
    pLink->SetAttribute("fromy", link.getFirstY());
    pLink->SetAttribute("tox", link.getSecondX());
    pLink->SetAttribute("toy", link.getSecondY());
    pLink->SetAttribute("direction", CLink::toString(link.getLinkDirection()).c_str());
    pLink->SetAttribute("activated", link.isActivated());
  }

  XMLElement *pEnemies = doc.NewElement("enemies");
  pMapElem->InsertEndChild(pEnemies);
  for (CEnemy *pEnemy : m_lEnemies) {
    XMLElement *pElem = doc.NewElement("enemy");
    pEnemies->InsertEndChild(pElem);
    pEnemy->writeToXMLElement(pElem);
  }

  XMLElement *pObjects = doc.NewElement("objects");
  pMapElem->InsertEndChild(pObjects);
  for (CObject *pObject : m_lObjects) {
    XMLElement *pElem = doc.NewElement("object");
    pObjects->InsertEndChild(pElem);
    pObject->writeToXMLElement(pElem);
  }

  XMLElement *pExit = doc.NewElement("exit");
  pMapElem->InsertEndChild(pExit);
  m_Exit.writeToXMLElement(pExit);

  XMLElement *pPlayer = doc.NewElement("player");
  pMapElem->InsertEndChild(pPlayer);
  m_pPlayer->writeToXMLElement(pPlayer);

  XMLElement *pCamera = doc.NewElement("camera");
  SetAttribute(pCamera, "pos", m_vCameraPos);
  pMapElem->InsertEndChild(pCamera);
  for (const CCameraRestriction &res : m_vCameraRestrictions) {
    XMLElement *pElem = doc.NewElement("restriction")            ;
    pCamera->InsertEndChild(pElem);
    res.writeToXMLElement(pElem);
  }

  XMLElement *pShots = doc.NewElement("shots");
  pMapElem->InsertEndChild(pShots);
  for (const CShot *pShot : m_lShots) {
    XMLElement *pElem = doc.NewElement("shot");
    pShots->InsertEndChild(pElem);
    pShot->writeToXMLElement(pElem);
  }
}
void CMap::readFromXMLElement(const tinyxml2::XMLElement *pRoot) {
  clearMap();
  
  m_pTutorialManager->setEnabled(BoolAttribute(pRoot, "tutorial", false));

  Ogre::String sBackground = pRoot->Attribute("background");
  if (sBackground.size() > 0) {
    m_pBackground = new CBackground(m_p2dManagerMap, m_vCameraPos, sBackground, m_vTilesPerScreen, m_fScreenRatio);
  }

  int sizeX = pRoot->IntAttribute("sizex");
  int sizeY = pRoot->IntAttribute("sizey");

  m_gridTiles.resize(sizeX, sizeY);

  unsigned int uiRow = 0;
  for (const XMLElement *pElement = pRoot->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement()) {
    if (std::string(pElement->Value()) == "tiles") {
      bool bInvert = BoolAttribute(pElement, "invert");
      for (const XMLElement *pRow = pElement->FirstChildElement(); pRow; pRow = pRow->NextSiblingElement()) {
	if (bInvert)
	  readRow(pRow, sizeY - 1 - uiRow);
	else
	  readRow(pRow, uiRow);
	++uiRow;
      }
    }
    else if (std::string(pElement->Value()) == "switches") {
      for (const XMLElement *pSwitch = pElement->FirstChildElement(); pSwitch; pSwitch = pSwitch->NextSiblingElement()){
	readSwitch(pSwitch);
      }
    }
    else if (std::string(pElement->Value()) == "endangeredTiles") {
      for (const XMLElement *pTile = pElement->FirstChildElement(); pTile; pTile = pTile->NextSiblingElement()) {
	readEndangeredTiles(pTile);
      }
    }
    else if (std::string(pElement->Value()) == "links") {
      for (const XMLElement *pLink = pElement->FirstChildElement(); pLink; pLink = pLink->NextSiblingElement()) {
	readLink(pLink);
      }
    }
    else if (std::string(pElement->Value()) == "enemies") {
      for (const XMLElement *pEnemy = pElement->FirstChildElement(); pEnemy; pEnemy = pEnemy->NextSiblingElement()) {
	m_lEnemies.push_back(new CEnemy(*this, pEnemy));
      }
    }
    else if (std::string(pElement->Value()) == "objects") {
      for (const XMLElement *pObject = pElement->FirstChildElement(); pObject; pObject = pObject->NextSiblingElement()) {
	m_lObjects.
	  push_back(new CObject(*this,
				Vector2Attribute(pObject),
				EnumAttribute<CObject::EObjectTypes>(pObject,
								     "type",
								     CObject::OT_COUNT)));
      }
    }
    else if (std::string(pElement->Value()) == "exit") {
      readExit(pElement);
    }
    else if (std::string(pElement->Value()) == "player") {
      assert(!m_pPlayer);

      m_pPlayer = new CPlayer(this, pElement, m_Statistics);
      playerWarped();
    }
    else if (std::string(pElement->Value()) == "camera") {
      readCamera(pElement);
    }
    else if (std::string(pElement->Value()) == "shots") {
      for (const XMLElement *pObject = pElement->FirstChildElement(); pObject; pObject = pObject->NextSiblingElement()) {
        addShot(new CShot(this, pObject));
      }
    }
  }
}
