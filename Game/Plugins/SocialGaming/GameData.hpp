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

#ifndef _GAME_DATA_HPP_
#define _GAME_DATA_HPP_

#include <vector>
#include <string>
#include "MissionState.hpp"

namespace SocialGaming {
  struct SLevelData {
    std::string sLevelName;
    MissionState::EMissionState eMissionState;
    bool bSkipped;

    SLevelData(const std::string &name,
	       MissionState::EMissionState ms = MissionState::MS_NOT_PLAYED,
	       bool skipped = false) 
      : sLevelName(name),
	eMissionState(ms),
	bSkipped(skipped) {
    }
  };

  class CLevelList : public std::vector<SLevelData> {
  public:
    const SLevelData *getData(const std::string &sLevelName) const {
      for (const SLevelData &data : *this) {
	if (data.sLevelName == sLevelName) {
	  return &data;
	}
      }
      return nullptr;
    }
    MissionState::EMissionState getMissionState(const std::string &sLevelName) const {
      const SLevelData *pData = getData(sLevelName);
      if (!pData) {
	return MissionState::MS_NOT_PLAYED;
      }
      return pData->eMissionState;
    }
    bool isSkipped(const std::string &sLevelName) const {
      const SLevelData *pData = getData(sLevelName);
      if (!pData) {
	return false;
      }
      return pData->bSkipped;
    }
  };

  class CGameData {
  public:
    virtual ~CGameData() {};
    virtual CLevelList getLevelList() = 0;
    virtual void setMissionStateOfLevel(MissionState::EMissionState eMissionState,
					const std::string &sLevelName,
					bool bSkipped) = 0;
    virtual void save() const = 0;
  };
};

#endif
