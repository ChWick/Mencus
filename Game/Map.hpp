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

class CTile;
class CPlayer;
class CSwitch;
class CShot;


class CMap : Ogre::FrameListener, public CSpriteTransformPipeline, public CInputListener {
public:
  enum ECollisionCheckDirections {
    CCD_TOP,
    CCD_LEFT,
    CCD_BOTTOM,
    CCD_RIGHT,
  };
  
private:
  std::list<CShot*> m_lShotsToDestroy;

  Ogre2dManager* m_p2dManagerMap;
  grid2d<CTile*> m_gridTiles;
  std::list<CSwitch*> m_lSwitches;
  std::list<CShot*> m_lShots;
  std::string m_sBackground;
  CSprite *m_pBackgroundSprite;

  Ogre::Vector2 m_vCameraPos;
  Ogre::Vector2 m_vCameraDebugOffset;

  CPlayer *m_pPlayer;

public:
  CMap(Ogre::SceneManager *pSceneManager);
  ~CMap();

  void loadMap(string sFilename);

  //! Function to check if a line penetrates into a tile a lower position
  /**
   * @param[in] uiCollisionCheckDirection in which direchtion to check
   * @param[in] uiTileMask a mask for tiles to match
   * @param[in] bb The bounding box to check for a hit
   * @return returns the penetration depth
   */
  Ogre::Real hitsTile(ECollisionCheckDirections eCollisionCheckDirection, unsigned int uiTileMask, const CBoundingBox2d &bb) const;
  //! Function to check if a bounding box hits a tile's bounding box
  /**
   *  \param[in] uiTileMask a mask for tiles to match
   *  \param[in] bb The bounding box to check for a hit
   *  \returns true if a hit occured
   */
  bool hitsTile(unsigned int uiTileMask, const CBoundingBox2d &bb) const;
  //! Function to check weather a bounding box is out of the map
  /**
   *  \param[in] bb The bounding box
   *  \returns Weather the bb is out of the map
   */
  bool outOfMap(const CBoundingBox2d &bb) const;
  //! Function to check if a switch is hit and to activate it
  /**
   * \param[in[ bb The bounding box to check for a hit
   */
  void activateSwitchOnHit(const CBoundingBox2d &bb);

  virtual bool frameStarted(const Ogre::FrameEvent& evt);

  // OIS::KeyListener
  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  Ogre::Vector2 transformPosition(const Ogre::Vector2 &vPosition) const;

  CTile* &getTile(unsigned int x, unsigned int y) {return m_gridTiles(x, y);}
  CTile *getTile(unsigned int x, unsigned int y) const {return m_gridTiles(x, y);}

  Ogre2dManager *get2dManager() const {return m_p2dManagerMap;}

  CShot *addShot(CShot *pShot) {m_lShots.push_back(pShot); return pShot;}
  void destroyShot(CShot *pShot) {m_lShotsToDestroy.push_back(pShot);}
private:
  void updateBackground(Ogre::Real tpf);
  void updateCameraPos();

  void clearMap();

  void readRow(tinyxml2::XMLElement *pRow, unsigned int uiRow);
  void readSwitch(tinyxml2::XMLElement *pSwitch);
};

#endif
