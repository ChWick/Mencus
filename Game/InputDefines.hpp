#ifndef _INPUT_DEFINES_
#define _INPUT_DEFINES_

#include "GlobalBuildDefines.hpp"

#ifdef INPUT_ENABLE_ALL

#  define INPUT_TOUCH
#  define INPUT_MOUSE
#  define INPUT_KEYBOARD

#elif ANDROID

#  define INPUT_TOUCH
#  define INPUT_KEYBOARD

#else

#  ifdef MAP_EDITOR_ENABLED
#    define INPUT_MOUSE
#    define INPUT_KEYBOARD
#  else
#    define INPUT_KEYBOARD
#    define INPUT_KEYBOARD_ONLY
#  endif

#endif


// make shure that input keyboard is defined when input keyboard only is defined
#ifdef INPUT_KEYBOARD_ONLY
#ifndef INPUT_KEYBOARD
#define INPUT_KEYBOARD
#endif
#endif

#endif
