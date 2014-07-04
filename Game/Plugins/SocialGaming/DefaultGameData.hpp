/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

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
