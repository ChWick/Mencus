#ifndef _TILE_TYPES_HPP_
#define _TILE_TYPES_HPP_

typedef unsigned int TileType;

enum ETileTypes {
  TT_NONE          = 0,
  TT_WALL_FRONT    = 1,
  TT_WALL_BACK     = 2,
  TT_DOOR_WOOD_TOP = 123,
  TT_DOOR_WOOD_BOT = 124,
  TT_COUNT         = 125,
};

#endif
