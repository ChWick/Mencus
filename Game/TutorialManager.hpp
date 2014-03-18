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
  const CMap &m_Map;
  std::vector<bool> m_bInformationShown;
public:
  CTutorialManager(const CMap &map);

  void start();

  void update();
};

#endif
