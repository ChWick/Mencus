#include "MissionState.hpp"
#include "Log.hpp"


namespace MissionState {
  std::string MISSION_STATE_STRINGS[MS_COUNT] = {
    "not_played",
    "failed",
    "skipped",
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
