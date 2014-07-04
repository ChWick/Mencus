#ifndef _MISSION_STATE_HPP_
#define _MISSION_STATE_HPP_

#include <string>

namespace MissionState {
  enum EMissionState {
    MS_NOT_PLAYED,	        //!< This mission has never been played 
    MS_FAILED,			//!< This mission has been failed
    MS_SKIPPED,			//!< This mission has been skipped
    MS_ACCOMPLISHED,		//!< This mission has been accomplished

    MS_COUNT,
  };

  EMissionState parseMissionState(const std::string &s);
  const std::string &toString(const EMissionState ms);
};

#endif
  
