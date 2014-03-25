#ifndef _TILE_TYPES_HPP_
#define _TILE_TYPES_HPP_

typedef unsigned int TileType;

enum ETileTypes {
  TT_NONE                 = 0,
  TT_WALL_FRONT           = 1,
  TT_WALL_BACK            = 2,
  TT_DOOR_WOOD_TOP_CLOSED = 123,
  TT_DOOR_WOOD_BOT_CLOSED = 124,
  TT_DOOR_WOOD_TOP_OPEN   = 125,
  TT_DOOR_WOOD_BOT_OPEN   = 126,
  TT_COUNT                = 127,
};

#endif
