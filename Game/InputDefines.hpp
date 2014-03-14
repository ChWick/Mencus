#ifndef _INPUT_DEFINES_
#define _INPUT_DEFINES_


#ifdef ANDROID
#define INPUT_TOUCH
#define INPUT_KEYBOARD
#else
#define INPUT_TOUCH
#define INPUT_MOUSE
#define INPUT_KEYBOARD
//#define INPUT_KEYBOARD_ONLY
#endif

#endif
