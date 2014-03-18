#ifndef _TUTORIAL_MANAGER_HPP_
#define _TUTORIAL_MANAGER_HPP_

#include <vector>

class CMap;

class CTutorialManager {
  enum EInformationStates {
    IS_MOTION = 0,
    IS_SWITCH,
    IS_ENEMY,
    IS_SMALL_ENEMY,

    IS_COUNT,
  };
private:
  bool m_bEnabled;
  const CMap &m_Map;
  std::vector<bool> m_bInformationShown;
public:
  CTutorialManager(const CMap &map);

  void setEnabled(bool bEnabled) {m_bEnabled = bEnabled;}

  void start();

  void update();
};

#endif
