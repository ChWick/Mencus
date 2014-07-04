#ifndef _DEFAULT_GAME_DATA_HPP_
#define _DEFAULT_GAME_DATA_HPP_

#include "GameData.hpp"

namespace SocialGaming {
  class CDefaultGameData : public CGameData {
  private:
    CLevelList m_LevelList;
  public:
    CDefaultGameData();
    ~CDefaultGameData();
    CLevelList getLevelList();
    void setMissionStateOfLevel(EMissionState eMissionState, std::string sLevelName);
  };
};

#endif
