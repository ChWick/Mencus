#ifndef TILE_HPP
#define TILE_HPP

#include "Sprite.hpp"

class Ogre2dManager;

typedef unsigned int TileType;

class CTile : public CSprite {
public:
  enum ETileTypes : TileType {
    TT_DEBUG         = 200,
  };
  enum ETileFlags {
    TF_UNPASSABLE    = 1,
    TF_PASSABLE      = 2,
    TF_DOOR1         = 4,
    TF_DOOR2         = 8,
    TF_DOOR3         = 16,
    TF_LOCK          = 32,
    TF_DAMAGES       = 64,
    TF_CHANGEBLOCK   = 128,
  };
private:
  static const Ogre::Vector2 DEFAULT_TILE_SIZE;
  static const string DEFAULT_TILE_TEXTURE_NAME;

  const unsigned int m_uiTileFlags;
  const TileType m_ttTileType;
public:
  CTile(CSpriteTransformPipeline *pTransformPipeline, Ogre2dManager* p2dManagerMap, const Ogre::Vector2 &vPosition, TileType ttTileType);
  CTile(const CTile &src);

  TileType getTileType() const {return m_ttTileType;}
  unsigned int getTileFlags() const {return m_uiTileFlags;}

private:
  unsigned int getTileFlags(TileType tt);
};

#endif
