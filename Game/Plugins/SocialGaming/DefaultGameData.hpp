#ifndef _DEFAULT_GAME_DATA_HPP_
#define _DEFAULT_GAME_DATA_HPP_

#include "GameData.hpp"

namespace SocialGaming {
  class CDefaultGameData : public CGameData {
  private:
    const std::string m_sFileName;
    CLevelList m_LevelList;
  public:
    CDefaultGameData();
    ~CDefaultGameData();
    CLevelList getLevelList();
    void setMissionStateOfLevel(MissionState::EMissionState eMissionState,
				const std::string &sLevelName);

    void read();
    void save() const;
  };
};

#endif
