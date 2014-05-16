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

#ifndef _INPUT_DEFINES_
#define _INPUT_DEFINES_

#include "GlobalBuildDefines.hpp"

#if ENABLE_INPUT_ALL

#  define INPUT_TOUCH
#  define INPUT_MOUSE
#  define INPUT_KEYBOARD

#elif ANDROID

#  define INPUT_TOUCH
#  define INPUT_KEYBOARD

#else

//#  if ENABLE_MAP_EDITOR
#    define INPUT_MOUSE
#    define INPUT_KEYBOARD
//#  else
//#    define INPUT_KEYBOARD
//#    define INPUT_KEYBOARD_ONLY
//#  endif

#endif


// make shure that input keyboard is defined when input keyboard only is defined
#ifdef INPUT_KEYBOARD_ONLY
#ifndef INPUT_KEYBOARD
#define INPUT_KEYBOARD
#endif
#endif

#endif
