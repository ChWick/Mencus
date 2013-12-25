#include "Map.hpp"
#include "Util.hpp"
#include <tinyxml2.h>
#include "InputListener.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "DebugDrawer.hpp"
#include "Switch.hpp"
#include "Shot.hpp"
#include "Explosion.hpp"
#include "Enemy.hpp"

using namespace tinyxml2;

const Ogre::Vector2 CTile::DEFAULT_TILE_SIZE(1, 1);
const string CTile::DEFAULT_TILE_TEXTURE_NAME = "../gfx/tiles/Tile";

const Ogre::Vector2 TILES_PER_SCREEN(16, 12);
const Ogre::Real SCREEN_RATIO = TILES_PER_SCREEN.y / TILES_PER_SCREEN.x;

CMap::CMap(Ogre::SceneManager *pSceneManager)
  : m_p2dManagerMap(NULL),
    m_pBackgroundSprite(NULL),
    m_pPlayer(NULL),
    m_vCameraDebugOffset(Ogre::Vector2::ZERO),
    m_vCameraPos(Ogre::Vector2::ZERO) {
  m_p2dManagerMap = new Ogre2dManager();
  m_p2dManagerMap->init(pSceneManager, Ogre::RENDER_QUEUE_BACKGROUND, true);
  Ogre::Root::getSingleton().addFrameListener(this);
  loadMap("../level/level1/scene3.xml");

  CInputListenerManager::getSingleton().addInputListener(this);

  m_pPlayer = new CPlayer(this, m_p2dManagerMap);
  m_pPlayer->setPosition(Ogre::Vector2(0, 2));

  new CDebugDrawer(this, m_p2dManagerMap);
}
CMap::~CMap() {
  clearMap();

  if (CDebugDrawer::getSingletonPtr()) {delete CDebugDrawer::getSingletonPtr();}
  if (m_pPlayer) {delete m_pPlayer; m_pPlayer = NULL;}
  CInputListenerManager::getSingleton().removeInputListener(this);
  Ogre::Root::getSingleton().removeFrameListener(this);
  m_p2dManagerMap->end();
  delete m_p2dManagerMap;
}
void CMap::clearMap() {
  m_lLinks.clear();
  m_lShotsToDestroy.clear();
  m_lExplosionsToDestroy.clear();
  m_lEnemiesToDestroy.clear();
  
  if (m_pBackgroundSprite) {
    delete m_pBackgroundSprite;
    m_pBackgroundSprite = NULL;
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

  for (auto pTile : m_gridTiles) {
    delete pTile;
  }
  m_gridTiles.clear();
}
void CMap::loadMap(string sFilename) {
  clearMap();
  
  XMLDocument doc;
  if (doc.LoadFile(sFilename.c_str())) {
    throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, sFilename + " not found.", __FILE__);
  }

  XMLElement *pRoot = doc.FirstChildElement("map");
  m_sBackground = pRoot->Attribute("background");
  if (m_sBackground.size() > 0) {
    m_sBackground = getBackgroundTexturePath(m_sBackground);
    m_pBackgroundSprite = new CSprite(&CBackgroundSpriteTransformPipeline::INSTANCE, m_p2dManagerMap, Ogre::Vector2(-1, -1), Ogre::Vector2(2, 2));
    m_pBackgroundSprite->setTexture(m_sBackground);
  }

  int sizeX = pRoot->IntAttribute("sizex");
  int sizeY = pRoot->IntAttribute("sizey");
  m_p2dManagerMap->spriteBltFull(
				 m_sBackground,
				 -1,
				 -1,
				 1,
				 1);

  m_gridTiles.resize(sizeX, sizeY);

  unsigned int uiRow = 0;
  for (XMLElement *pElement = pRoot->FirstChildElement(); pElement; pElement = pElement->NextSiblingElement()) {
    if (std::string(pElement->Value()) == "tiles") {
      bool bInvert = pElement->BoolAttribute("invert");
      for (XMLElement *pRow = pElement->FirstChildElement(); pRow; pRow = pRow->NextSiblingElement()) {
	if (bInvert)
	  readRow(pRow, sizeY - 1 - uiRow);
	else
	  readRow(pRow, uiRow);
	++uiRow;
      }
    }
    else if (std::string(pElement->Value()) == "switches") {
      for (XMLElement *pSwitch = pElement->FirstChildElement(); pSwitch; pSwitch = pSwitch->NextSiblingElement()){
	readSwitch(pSwitch);
      }
    }
    else if (std::string(pElement->Value()) == "endangeredTiles") {
      for (XMLElement *pTile = pElement->FirstChildElement(); pTile; pTile = pTile->NextSiblingElement()) {
	readEndangeredTiles(pTile);
      }
    }
    else if (std::string(pElement->Value()) == "links") {
      for (XMLElement *pLink = pElement->FirstChildElement(); pLink; pLink = pLink->NextSiblingElement()) {
	readLink(pLink);
      }
    }
    else if (std::string(pElement->Value()) == "enemies") {
      for (XMLElement *pEnemy = pElement->FirstChildElement(); pEnemy; pEnemy = pEnemy->NextSiblingElement()) {
	readEnemy(pEnemy);
      }
    }
  }


  // Initialise everything
  for (auto pSwitch : m_lSwitches) {
    pSwitch->initialize(this);
  }
}
Ogre::Real CMap::hitsTile(ECollisionCheckDirections eCollisionCheckDirection, unsigned uiTileMask, const CBoundingBox2d &bb) const {
  // loop trough tiles that could be hit
  int startX = static_cast<int>(bb.getPosition().x + ((eCollisionCheckDirection == CCD_RIGHT) ? bb.getSize().x : 0)); // round down
  int startY = static_cast<int>(bb.getPosition().y + ((eCollisionCheckDirection == CCD_TOP) ? bb.getSize().y : 0)); // round down

 
  if (eCollisionCheckDirection == CCD_TOP || eCollisionCheckDirection == CCD_BOTTOM) {
    int endX = static_cast<int>(bb.getPosition().x + bb.getSize().x + 0.99); // round up

    for (int x = startX; x < endX; ++x) {
      if ((m_gridTiles(x, startY)->getTileFlags() & uiTileMask) == uiTileMask) {
#ifdef DEBUG_COLLISION_WITH_TILES
	CDebugDrawer::getSingleton().draw(m_gridTiles(x, startY));
#endif
	return bb.getPosition().y - startY - ((eCollisionCheckDirection == CCD_BOTTOM) ? 1 : -bb.getSize().y); // Collision, so penetration
      }
    }
  }
  else {
    int endY = static_cast<int>(bb.getPosition().y + bb.getSize().y + 0.99); // round up

    for (int y = startY; y < endY; ++y) {
      if ((m_gridTiles(startX, y)->getTileFlags() & uiTileMask) == uiTileMask) {
#ifdef DEBUG_COLLISION_WITH_TILES
	CDebugDrawer::getSingleton().draw(m_gridTiles(startX, y));
#endif
	return bb.getPosition().x - startX - ((eCollisionCheckDirection == CCD_LEFT) ? 1 : -bb.getSize().x); // Collision, so penetration
      }
    }
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
bool CMap::outOfMap(const CBoundingBox2d &bb) const {
	  if (bb.getPosition().x + bb.getSize().x < 0) return true;
	  if (bb.getPosition().y + bb.getSize().y < 0) return true;
	  if (bb.getPosition().x > m_gridTiles.getSizeX()) return true;
	  if (bb.getPosition().y > m_gridTiles.getSizeY()) return true;
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
void CMap::explodeTiles(const Ogre::Vector2 &vCenter, Ogre::Real r) {
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
    if (((m_gridTiles(link.getFirstX(), link.getFirstY())->getTileFlags() & (CTile::TF_DOOR1 | CTile::TF_DOOR2 | CTile::TF_DOOR3)) == 0) ||
	((m_gridTiles(link.getSecondX(), link.getSecondY())->getTileFlags() & (CTile::TF_DOOR1 | CTile::TF_DOOR2 | CTile::TF_DOOR3)) == 0)) {
      continue;
    }
    if (m_gridTiles(link.getFirstX(), link.getFirstY())->getWorldBoundingBox().collidesWith(bb) != CCD_NONE) {
      vFromPos = Ogre::Vector2(link.getFirstX(), link.getFirstY());
      vToPos   = Ogre::Vector2(link.getSecondX(), link.getSecondY());
      return true;
    }
    else if (m_gridTiles(link.getSecondX(), link.getSecondY())->getWorldBoundingBox().collidesWith(bb) != CCD_NONE) {
      vFromPos = Ogre::Vector2(link.getSecondX(), link.getSecondY());
      vToPos   = Ogre::Vector2(link.getFirstX(), link.getFirstY());
      return true;
    }
  }

  return false;
}
bool CMap::keyPressed( const OIS::KeyEvent &arg ) {
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

  return true;
}
bool CMap::keyReleased( const OIS::KeyEvent &arg ) {
  return true;
}
bool CMap::frameStarted(const Ogre::FrameEvent& evt) {
  updateCameraPos();

  // order of updates exquates drawing order, last one will be on top
  updateBackground(evt.timeSinceLastFrame);
  for (auto pTile : m_gridTiles) {
    pTile->update(evt.timeSinceLastFrame);
  }
  for (auto pEnemy : m_lEnemies) {
    pEnemy->update(evt.timeSinceLastFrame);
  }
  for (auto pSwitch : m_lSwitches) {
    pSwitch->update(evt.timeSinceLastFrame);
  }
  for (auto pShot : m_lShots) {
    pShot->update(evt.timeSinceLastFrame);
  }

  m_pPlayer->update(evt.timeSinceLastFrame);

  for (auto pExplosion : m_lExplosions) {
    pExplosion->update(evt.timeSinceLastFrame);
  }

  // draw links if debug
#ifdef DEBUG_LINKS
  for (auto &link : m_lLinks) {
    CDebugDrawer::getSingleton().draw(link);
  }
#endif

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

  return true;
}
void CMap::updateBackground(Ogre::Real tpf) {
  if (m_pBackgroundSprite) {
    m_pBackgroundSprite->setTextureCoords(Ogre::Vector2(0, 0), Ogre::Vector2(1.5 / SCREEN_RATIO, 1.5));
    m_pBackgroundSprite->update(tpf);
  }
}
void CMap::updateCameraPos() {
  Ogre::Vector2 vCenter = m_pPlayer->getPosition() + m_pPlayer->getSize() / 2;
  m_vCameraPos = vCenter - TILES_PER_SCREEN/ 2;
  m_vCameraPos.x = max(m_vCameraPos.x, static_cast<Ogre::Real>(0));
  m_vCameraPos.y = max(m_vCameraPos.y, static_cast<Ogre::Real>(0));

  m_vCameraPos.x = min(m_vCameraPos.x, m_gridTiles.getSizeX() - TILES_PER_SCREEN.x);
  m_vCameraPos.y = min(m_vCameraPos.y, m_gridTiles.getSizeY() - TILES_PER_SCREEN.y);
}
Ogre::Vector2 CMap::transformPosition(const Ogre::Vector2 &vPosition) const {
  Ogre::Vector2 vOffset(m_vCameraPos + m_vCameraDebugOffset);
  return ((vPosition - vOffset) / TILES_PER_SCREEN * 2 - Ogre::Vector2(1, 1));
}
void CMap::readRow(XMLElement *pRow, unsigned int uiRow) {
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
void CMap::readSwitch(XMLElement *pSwitch) {
  CSwitch *pNewSwitch = new CSwitch(this,
				 m_p2dManagerMap,
				 Ogre::Vector2(pSwitch->FloatAttribute("x"),
					       pSwitch->FloatAttribute("y")),
				 pSwitch->IntAttribute("type"));
  for (XMLElement *pChange = pSwitch->FirstChildElement(); pChange; pChange = pChange->NextSiblingElement()) {
    SSwitchEntry entry;
    entry.uiTileType = pChange->IntAttribute("id");
    entry.uiTilePosX = pChange->IntAttribute("x");
    entry.uiTilePosY = pChange->IntAttribute("y");

    pNewSwitch->addEntry(entry);
  }
  
  m_lSwitches.push_back(pNewSwitch);
}
void CMap::readEndangeredTiles(XMLElement *pTile) {
  TileType tt = pTile->IntAttribute("targetTile");
  size_t x = pTile->IntAttribute("x");
  size_t y = pTile->IntAttribute("y");

  m_gridTiles(x, y)->setEndangeredTileType(tt);
  
  Ogre::LogManager::getSingleton().logMessage("Parsed endangered tile at (" + Ogre::StringConverter::toString(x) + ", " + Ogre::StringConverter::toString(y) + ") with target tile type " + Ogre::StringConverter::toString(tt));
}

void CMap::readLink(XMLElement *pLink) {
  m_lLinks.push_back(CLink(
			   pLink->IntAttribute("fromx"),
			   pLink->IntAttribute("fromy"),
			   pLink->IntAttribute("tox"),
			   pLink->IntAttribute("toy")));
  Ogre::LogManager::getSingleton().logMessage("Parsed: " + m_lLinks.back().toString());
}
void CMap::readEnemy(XMLElement *pEnemy) {
  CEnemy::EEnemyTypes eEnemyType = static_cast<CEnemy::EEnemyTypes>(pEnemy->IntAttribute("id") - 1);
  Ogre::Vector2 vPos(pEnemy->FloatAttribute("x"), pEnemy->FloatAttribute("y"));
  Ogre::Real fDirection(pEnemy->FloatAttribute("direction"));

  CEnemy *pNewEnemy = new CEnemy(*this, vPos, eEnemyType, fDirection);
  m_lEnemies.push_back(pNewEnemy);

  Ogre::LogManager::getSingleton().logMessage("Parsing Enemy at " + Ogre::StringConverter::toString(vPos));
}
