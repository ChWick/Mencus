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

#ifndef MAP_H
#define MAP_H

#include <Ogre.h>
#include <list>
#include "DebugDefines.hpp"
#include "ogre2d-main.hpp"
#include "Sprite.hpp"
#include "Grid2d.hpp"
#include "SpriteTransformPipeline.hpp"
#include <tinyxml2.h>
#include "InputListener.hpp"
#include "BoundingBox2d.hpp"
#include "CollisionDefines.hpp"
#include "Link.hpp"
#include "ScreenplayListener.hpp"
#include "PauseListener.hpp"
#include "CameraRestriction.hpp"
#include "MapPack.hpp"
#include "Exit.hpp"
#include "OutputStyle.hpp"
#include "TileTypes.hpp"
#include "Entity.hpp"

class CTile;
class CPlayer;
class CSwitch;
class CShot;
class CExplosion;
class CEnemy;
class CObject;
class CBackground;
class CTutorialManager;
class CDebugText;
struct SStatistics;

class CMap : public CEntity,
	     public CSpriteTransformPipeline,
	     public CInputListener,
	     public CPauseListener {
private:
  CTutorialManager *m_pTutorialManager;
  std::list<CEntity*> m_lEntitiesToDestroy;
  Ogre2dManager* m_p2dManagerMap;
  Ogre2dManager* m_pDebugSpriteManager;
  grid2d<CTile*> m_gridTiles;
  std::vector<CDebugText*> m_vLineNumberX;
  std::vector<CDebugText*> m_vLineNumberY;
  std::vector<CCameraRestriction> m_vCameraRestrictions;

  CBackground *m_pBackground;
  CEntity *m_pTilesEntity;
  CEntity *m_pEnemiesEntity;
  CEntity *m_pSwitchesEntity;
  CEntity *m_pShotsEntity;
  CEntity *m_pExplosionsEntity;
  CEntity *m_pLinksEntity;
  CEntity *m_pObjectsEntity;
  CEntity *m_pOthersEntity;

  Ogre::Vector2 m_vTilesPerScreen;
  float m_fScreenRatio;
  Ogre::Vector2 m_vMapSize;
  Ogre::Vector2 m_vTransformedStartPos;
  Ogre::Vector2 m_vTransformedEndPos;
  Ogre::Vector2 m_vCameraPos;
  Ogre::Vector2 m_vCameraTargetPos;
  Ogre::Vector2 m_vCameraDebugOffset;
  Ogre::Vector2 m_vBlackBarSizeBottomLeft;
  Ogre::Vector2 m_vBlackBarSizeTopRight;

  CPlayer *m_pPlayer;
  CExit m_Exit;

  CScreenplayListener *m_pScreenplayListener;
  bool m_bUpdatePause;
  bool m_bRenderPause;
  Ogre::Real m_fPlayingTime;
  SStatistics &m_Statistics;
  const CMapPackConstPtr m_pMapPack;
public:
  CMap(Ogre::SceneManager *pSceneManager,
       CScreenplayListener *pScreenplayListener,
       SStatistics &statistics,
       const CMapPackConstPtr m_pMapPack);
  ~CMap();

  void init();

  void loadMap(const string &sFilename, const string &sResourceGroup);
  void loadMap(const CMapPackConstPtr pMapPack);
  void prepareMap();
  void writeToXMLElement(tinyxml2::XMLElement *pMapElem, EOutputStyle eStype) const;
  void readFromXMLElement(const tinyxml2::XMLElement *pRoot);
  void resize(const Ogre::Vector2 &vSize, const Ogre::Vector2 &vOrigin = Ogre::Vector2::ZERO);
  void resizeTiles(unsigned int uiSizeX, unsigned int uiSizeY);
  void resizeTilesPerScreen(const Ogre::Vector2 &tps);

  //! Function to check if a line penetrates into a tile a lower position
  /**
   * @param[in] uiCollisionCheckDirection in which direchtion to check
   * @param[in] uiTileMask a mask for tiles to match
   * @param[in] bb The bounding box to check for a hit
   * @param[in] ppTile the collided Tile
   * @return returns the penetration depth
   */
  Ogre::Real hitsTile(ECollisionCheckDirections eCollisionCheckDirection, unsigned int uiTileMask, const CBoundingBox2d &bb, CTile **ppTile = NULL) const;
  //! Function to check if a bounding box hits a tile's bounding box
  /**
   *  \param[in] uiTileMask a mask for tiles to match
   *  \param[in] bb The bounding box to check for a hit
   *  \param[out] pbbOverlap If not zero, the overlap of the hit with the collided bb will be stored
   *  \param[out] ppTile the collided Tile
   *  \returns the collision direction or CCD_NONE
   */
  unsigned int hitsTile(unsigned int uiTileMask,
			const CBoundingBox2d &bb,
			CBoundingBox2d *pbbOverlap = NULL,
			CTile **ppTile = NULL) const;
  //! Function to check whether a bounding box is out of the map
  /**
   *  \param[in] bb The bounding box
   *  \param[in] eCollisionCheckDirection If only one particular direction shall be checked
   *  \returns Whether the bb is out of the map
   */
  bool outOfMap(const CBoundingBox2d &bb, ECollisionCheckDirections eCollisionCheckDirection = CCD_ALL) const;
  bool outOfMap(int x, int y) const {return x < 0 || y < 0 || x >= static_cast<int>(m_gridTiles.getSizeX()) || y >= static_cast<int>(m_gridTiles.getSizeY());}
  //! Function to check whether a bounding box collides with the map margin
  /**
   *  \param[in] bb The bounding box
   *  \returns Whether the bb collides with the map margin
   */
  bool collidesWithMapMargin(const CBoundingBox2d &bb) const;
  //! Function to check if a switch is hit and to activate it
  /**
   * \param[in] bb The bounding box to check for a hit
   */
  void activateSwitchOnHit(const CBoundingBox2d &bb);
  //! Function to call if a bomb explodes
  /** This function will search for tiles in a given radius,
   * that are hit by this bomb.
   * \param[in] vCenter the explosion center
   * \param[in] r The radius of the explosion around the center
   */
  void createExplosion(const Ogre::Vector2 &vCenter, Ogre::Real r);
  //! Function to let a tile explode if it is endangered
  /**
   * \param[in] x The x coord of the tile
   * \param[in] y The y coord of the tile
   * \param[in] bExplodeNeighbours Let nighbours explode aswell
   */
  void explodeTile(size_t x, size_t y, bool bExplodeNeighbours);
  //! Function to find a link at the given position
  /**
   *  \param[in] bb The bounding box to check for collision
   *  \param[out] The position of the entrance
   *  \param[out] The position of the exit
   *  \returns true if there was a collision
   */
  bool findLink(const CBoundingBox2d &bb, Ogre::Vector2 &vFromPos, Ogre::Vector2 &vToPos) const;
  //! Function to check if the player is visible from a certain point
  /**
   *  \param[in] vFromPos from pos
   *  \param[in] fMaxDistance the maximal distance the check
   *  \param[in] eCollisionCheckDirection in witch direction to look
   *  \return The look direction in witch the player was visible
   */
  ECollisionCheckDirections playerVisibleFromPoint(const Ogre::Vector2 &vFromPos, Ogre::Real fMaxDistance, ECollisionCheckDirections eCollisionCheckDirection) const;

  void unlock(unsigned int x, unsigned int y);
  void swapBoxes();
  //! Function to change the tile type of a tile
  /**
   *  \param[in] uiTilePosX x position of the tile
   *  \param[in] uiTilePosY y position of the tile
   *  \param[in] ttTileType the new tile type
   */
  void changeTileType(unsigned int uiTilePosX, unsigned int uiTilePosY, TileType ttTileType);

  void update(Ogre::Real tpf);
  void render(Ogre::Real tpf);

  // OIS::KeyListener
  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  // SpriteTransformPipeline
  Ogre::Vector2 transformPosition(const Ogre::Vector2 &vPosition) const;
  bool isVisible(const Ogre::Vector2 &vPosition) const ;
  bool isVisible(const Ogre::Vector2 &vStart, const Ogre::Vector2 &vEnd) const;

  void setLineNumbersVisible(bool bVisible);

  const CMapPackConstPtr getMapPack() const {return m_pMapPack;}
  CTile* &getTile(unsigned int x, unsigned int y) {return m_gridTiles(x, y);}
  CTile *getTile(unsigned int x, unsigned int y) const {return m_gridTiles(x, y);}
  const grid2d<CTile*> &getTilesGrid() const {return m_gridTiles;}
  CLink *getLinkById(const Ogre::String &id);
  CEnemy *getEnemyById(const Ogre::String &id);
  const std::list<CEntity*> &getSwitches() const {return m_pSwitchesEntity->getChildren();}
  const std::list<CEntity*> &getEnemies() const {return m_pEnemiesEntity->getChildren();}
  const std::list<CEntity*> &getLinks() const {return m_pLinksEntity->getChildren();}
  const std::list<CEntity*> &getObjects() const {return m_pObjectsEntity->getChildren();}
  const std::list<CEntity*> &getShots() const {return m_pShotsEntity->getChildren();}

  SStatistics &getStatistics() {return m_Statistics;}
  const SStatistics &getStatistics() const {return m_Statistics;}

  CExit &getExit() {return m_Exit;}

  bool isInMap(unsigned int x, unsigned int y);

  Ogre2dManager *get2dManager() const {return m_p2dManagerMap;}
  Ogre2dManager *getDebug2dManager() const {return m_pDebugSpriteManager;}
  CPlayer *getPlayer() const {return m_pPlayer;}


  CEntity *getTilesEntity() {return m_pTilesEntity;}
  CEntity *getEnemiesEntity() {return m_pEnemiesEntity;}
  CEntity *getSwitchesEntity() {return m_pSwitchesEntity;}
  CEntity *getShotsEntity() {return m_pShotsEntity;}
  CEntity *getExplosionsEntity() {return m_pExplosionsEntity;}
  CEntity *getLinksEntity() {return m_pLinksEntity;}
  CEntity *getObjectsEntity() {return m_pObjectsEntity;}

  void destroy(CEntity *pEntity, bool bLater = true);

  void playerWarped();

  Ogre::Vector2 mouseToMapPos(const Ogre::Vector2 &vMousePos) const;
  Ogre::Vector2 mouseToMapSize(const Ogre::Vector2 &vMouseSize) const;
  Ogre::Vector2 mapToRelativeScreenPos(const Ogre::Vector2 &vMapPos) const;

  void setCameraPos(const Ogre::Vector2 &vPos) {m_vCameraTargetPos = m_vCameraPos = vPos;}
  const Ogre::Vector2 &getCameraTargetPos() const {return m_vCameraTargetPos;}
  void translateCamera(const Ogre::Vector2 &vOffset) {m_vCameraTargetPos = m_vCameraPos += vOffset;}

  // PauseListener
  virtual void mapUpdatePauseChanged(bool bPause) {m_bUpdatePause = bPause;}
  virtual void mapRenderPauseChanged(bool bPause);

protected:
  virtual void handleMessage(const CMessage &message);
private:
  void updateBackground(Ogre::Real tpf);
  void renderBackground(Ogre::Real tpf);
  void updateCameraPos(Ogre::Real tpf);

  void clearMap();
  void clearLineNumbers();

  void readRow(const tinyxml2::XMLElement *pRow, unsigned int uiRow);
  void readEndangeredTiles(const tinyxml2::XMLElement *pTile);
  void readExit(const tinyxml2::XMLElement *pExit);
  void readCamera(const tinyxml2::XMLElement *pCamera);
};

#endif
