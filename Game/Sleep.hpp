#ifndef _SLEEP_HPP_
#define _SLEEP_HPP_

// sleep functions
#if _WIN32
#include <windows.h>
void usleep(unsigned long us);
#else
// linux/android
#include <unistd.h>
#endif

#endif
