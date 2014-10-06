#include "Sleep.hpp"

#if _WIN32
void usleep(unsigned long us) {
  return Sleep(static_cast<DWORD>(us / 1000));
}
#endif
