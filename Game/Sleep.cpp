#include "Sleep.hpp"

void usleep(unsigned long us) {
  return Sleep(static_cast<DWORD>(us / 1000));
}
