#ifndef DEBUG_DEFINES_H
#define DEBUG_DEFINES_H

#ifdef ANDROID
#define DEBUG_SHOW_OGRE_TRAY
#else
#define DEBUG_SHOW_OGRE_TRAY
#define DEBUG_COLLISION_WITH_TILES
#define DEBUG_CHARACTER_BOUNDING_BOXES
#define DEBUG_ENDANGERED_TILES
#define DEBUG_LINKS
#define DEBUG_KI
#define DEBUG_EXIT
#define DEBUG_PLAYER_NO_COLLISION
#endif

#endif
