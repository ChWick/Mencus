#ifndef PAUSE_CALLER_HPP
#define PAUSE_CALLER_HPP

#include "PauseManager.hpp"

class CPauseCaller {
  unsigned int m_uiPausedFlags;
public:
  CPauseCaller()
    : m_uiPausedFlags(PAUSE_NONE) {
    CPauseManager::getSingleton().addCaller(this);
  }
  ~CPauseCaller() {
    CPauseManager::getSingleton().removeCaller(this);
  }
  void pause(unsigned int uiPauseFlags = PAUSE_ALL) {
    m_uiPausedFlags |= uiPauseFlags;
  }
  void unpause(unsigned int uiPauseFlags = PAUSE_ALL) {
    m_uiPausedFlags ^= uiPauseFlags & m_uiPausedFlags;
  }
  void setPause(unsigned int uiPauseFlags) {
    m_uiPausedFlags = uiPauseFlags;
  }
  unsigned int getPause() const {return m_uiPausedFlags;}
};

#endif // PAUSE_CALLER_HPP
