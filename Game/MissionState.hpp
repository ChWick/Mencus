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

#ifndef _MISSION_STATE_HPP_
#define _MISSION_STATE_HPP_

#include <string>

namespace MissionState {
  enum EMissionState {
    MS_NOT_PLAYED,	        //!< This mission has never been played 
    MS_FAILED,			//!< This mission has been failed
    MS_ACCOMPLISHED,		//!< This mission has been accomplished

    MS_COUNT,
  };

  EMissionState parseMissionState(const std::string &s);
  const std::string &toString(const EMissionState ms);
};

#endif
  
