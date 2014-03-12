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

class CTile;
class CPlayer;
class CSwitch;
class CShot;
class CExplosion;
class CEnemy;
class CObject;
class CBackground;

class CMap : public CSpriteTransformPipeline, public CInputListener, public CPauseListener {
private:
  enum EExitTypes {
    EXIT_REGION,
    EXIT_ENEMY_DEATH,
  };
  class CExit {
  public:
    static Ogre::String toString(EExitTypes et);
  private:
    EExitTypes m_eExitType;
    CBoundingBox2d m_BoundingBox;
    const Ogre::String m_sID;
  public:
    static CExit *newRegion(const CBoundingBox2d &bb) {return new CExit(EXIT_REGION, bb, Ogre::StringUtil::BLANK);}
    static CExit *newEnemyDeath(const Ogre::String &sID) {return new CExit(EXIT_ENEMY_DEATH, CBoundingBox2d(), sID);}
  private:
    CExit(EExitTypes eExitType, const CBoundingBox2d &bb, const Ogre::String &sID)
    : m_eExitType(eExitType), m_BoundingBox(bb), m_sID(sID) {
    }
  public:
    bool isInExit(CPlayer *pPlayer, CMap *pMap);
#ifdef DEBUG_EXIT
    void debugDraw();
#endif
    virtual void writeToXMLElement(tinyxml2::XMLElement *pElem) const;
  };
public:
private:
  std::list<CEnemy*> m_lEnemiesToDestroy;
  std::list<CShot*> m_lShotsToDestroy;
  std::list<CExplosion*> m_lExplosionsToDestroy;
  std::list<CObject*> m_lObjectsToDestroy;
  Ogre2dManager* m_p2dManagerMap;
  Ogre2dManager* m_pDebugSpriteManager;
  grid2d<CTile*> m_gridTiles;
  std::list<CEnemy*> m_lEnemies;
  std::list<CSwitch*> m_lSwitches;
  std::list<CShot*> m_lShots;
  std::list<CExplosion*> m_lExplosions;
  std::list<CLink> m_lLinks;
  std::list<CObject*> m_lObjects;
  std::vector<CCameraRestriction> m_vCameraRestrictions;

  CBackground *m_pBackground;

  Ogre::Vector2 m_vCameraPos;
  Ogre::Vector2 m_vCameraTargetPos;
  Ogre::Vector2 m_vCameraDebugOffset;

  CPlayer *m_pPlayer;
  CExit *m_pExit;

  CScreenplayListener *m_pScreenplayListener;
  bool m_bUpdatePause;
  bool m_bRenderPause;
public:
  CMap(Ogre::SceneManager *pSceneManager, CScreenplayListener *pScreenplayListener);
  ~CMap();

  void loadMap(const string &sFilename, const string &sResourceGroup);
  void writeToXMLElement(tinyxml2::XMLElement *pMapElem) const;
  void readFromXMLElement(tinyxml2::XMLElement *pRoot);

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

  void update(Ogre::Real tpf);
  void render(Ogre::Real tpf);

  // OIS::KeyListener
  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  Ogre::Vector2 transformPosition(const Ogre::Vector2 &vPosition) const;

  CTile* &getTile(unsigned int x, unsigned int y) {return m_gridTiles(x, y);}
  CTile *getTile(unsigned int x, unsigned int y) const {return m_gridTiles(x, y);}
  CLink *getLinkById(const Ogre::String &id);
  CEnemy *getEnemyById(const Ogre::String &id);

  bool isInMap(unsigned int x, unsigned int y);

  Ogre2dManager *get2dManager() const {return m_p2dManagerMap;}
  CPlayer *getPlayer() const {return m_pPlayer;}

  CShot *addShot(CShot *pShot) {m_lShots.push_back(pShot); return pShot;}
  void destroyShot(CShot *pShot) {m_lShotsToDestroy.push_back(pShot);}

  CExplosion *addExplosion(CExplosion *pExplosion) {m_lExplosions.push_back(pExplosion); return pExplosion;}
  void destroyExplosion(CExplosion *pExplosion) {m_lExplosionsToDestroy.push_back(pExplosion);}


  void destroyEnemy(CEnemy *pEnemy);

  void destroyObject(CObject *pObject) {if (find(m_lObjectsToDestroy.begin(), m_lObjectsToDestroy.end(), pObject) == m_lObjectsToDestroy.end()) m_lObjectsToDestroy.push_back(pObject);}

  const std::list<CEnemy*> &getEnemies() const {return m_lEnemies;}

  void playerWarped();

  // PauseListener
  virtual void mapUpdatePauseChanged(bool bPause) {m_bUpdatePause = bPause;}
  virtual void mapRenderPauseChanged(bool bPause) {m_bRenderPause = bPause;}
private:
  void updateBackground(Ogre::Real tpf);
  void renderBackground(Ogre::Real tpf);
  void updateCameraPos(Ogre::Real tpf);

  void clearMap();

  void readRow(tinyxml2::XMLElement *pRow, unsigned int uiRow);
  void readSwitch(tinyxml2::XMLElement *pSwitch);
  void readEndangeredTiles(tinyxml2::XMLElement *pTile);
  void readLink(tinyxml2::XMLElement *pLink);
  void readEnemy(tinyxml2::XMLElement *pEnemy);
  void readObject(tinyxml2::XMLElement *pObject);
  void readExit(tinyxml2::XMLElement *pExit);
  void readPlayer(tinyxml2::XMLElement *pPlayerElem);
  void readCamera(tinyxml2::XMLElement *pCamera);
};

#endif
