#ifndef TILE_HPP
#define TILE_HPP

#include "Sprite.hpp"
#include "TileTypes.hpp"

class Ogre2dManager;
class CMap;


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
    TF_ENDANGERED    = 256,
  };
  static const Ogre::Vector2 DEFAULT_TILE_SIZE;
private:
  static const string DEFAULT_TILE_TEXTURE_NAME;

  const unsigned int m_uiMapPosX;
  const unsigned int m_uiMapPosY;

  unsigned int m_uiTileFlags;
  const TileType m_ttTileType;

  TileType m_ttEndangeredTileType;
public:
  CTile(CMap &map, const Ogre::Vector2 &vPosition, TileType ttTileType);
  CTile(const CTile &src);

  void update(Ogre::Real tpf);
  TileType getTileType() const {return m_ttTileType;}
  unsigned int getTileFlags() const {return m_uiTileFlags;}

  TileType getEndangeredTileType() const {return m_ttEndangeredTileType;}
  TileType &getEndangeredTileType() {return m_ttEndangeredTileType;}
  void setEndangeredTileType(TileType tt);

  unsigned int getMapPosX() const {return m_uiMapPosX;}
  unsigned int getMapPosY() const {return m_uiMapPosY;}

private:
  unsigned int getTileFlags(TileType tt);
};

#endif
