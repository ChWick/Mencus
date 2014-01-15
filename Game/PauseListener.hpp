#ifndef PAUSE_LISTENER_HPP
#define PAUSE_LISTENER_HPP

#include "PauseManager.hpp"

class CPauseListener {
public:
  CPauseListener() {
    CPauseManager::getSingleton().addListener(this);
  }
  virtual ~CPauseListener() {
    CPauseManager::getSingleton().removeListener(this);
  }

  virtual void playerInputPauseChanged(bool bPause) {}
  virtual void playerMovePauseChanged(bool bPause) {}
  virtual void enemyMovePauseChanged(bool bPause) {}
  virtual void videoPauseChanged(bool bPause) {}
};

#endif // PAUSE_LISTENER_HPP
