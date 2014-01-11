#ifndef PAUSE_LISTENER_HPP
#define PAUSE_LISTENER_HPP

class CPauseListener {
public:
  void playerInputPauseChanged(bool bPause) {}
  void playerMovePauseChanged(bool bPause) {}
  void enemyMovePauseChanged(bool bPause) {}
};

#endif // PAUSE_LISTENER_HPP
