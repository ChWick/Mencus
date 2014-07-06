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

#include "MissionState.hpp"
#include "Log.hpp"


namespace MissionState {
  std::string MISSION_STATE_STRINGS[MS_COUNT] = {
    "not_played",
    "failed",
    "accomplished"
  };

  EMissionState parseMissionState(const std::string &s) {
    for (int i = 0; i < MS_COUNT; i++) {
      if (MISSION_STATE_STRINGS[i] == s) {
	return static_cast<EMissionState>(i);
      }
    }
    
    LOGW("'%s' is not a valid mission state to parse", s.c_str());
    
    return MS_NOT_PLAYED;
  }
  const std::string &toString(const EMissionState ms) {
    return MISSION_STATE_STRINGS[ms];
  }
};
