#ifndef _GAME_INPUT_LISTENER_HPP_
#define _GAME_INPUT_LISTENER_HPP_

class CGameInputCommand;

class CGameInputListener {
private:
  bool m_bGameInputListenerEnabled;
public:
CGameInputListener(bool bDefaultEnabled = true);
~CGameInputListener();
  virtual void receiveInputCommand(const CGameInputCommand &cmd) = 0;

void setGameInputListenerEnabled(bool bEnabled) {
m_bGameInputListenerEnabled = bEnabled;}
bool isGameInputListenerEnabled() const {
return m_bGameInputListenerEnabled;}
};

#endif
