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
  TT_LINK_WOOD_TOP        = 127,
  TT_LINK_WOOD_BOT        = 128,
  TT_LINK_DOOR_CASTLE_TOP_CLOSED   = 129,
  TT_LINK_DOOR_CASTLE_BOT_CLOSED   = 130,
  
  TT_COUNT                = 131,
};

#endif
