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

#ifndef DEBUG_DEFINES_H
#define DEBUG_DEFINES_H

#include "GlobalBuildDefines.hpp"

#if ENABLE_DEBUG_DEFINES

#ifdef ANDROID

//#define DEBUG_SHOW_OGRE_TRAY

#else

#define DEBUG_SHOW_OGRE_TRAY
#define DEBUG_COLLISION_WITH_TILES
#define DEBUG_CHARACTER_BOUNDING_BOXES
#define DEBUG_ENDANGERED_TILES
#define DEBUG_LINKS
#define DEBUG_KI
#define DEBUG_EXIT
#define DEBUG_PLAYER_NO_COLLISION
#define DEBUG_LOAD_SAFE
#define DEBUG_REGION

#endif

#endif // USE_DEBUG_DEFINES

#endif // DEBUG_DEFINES_H
