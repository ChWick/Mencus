#ifndef _STATISTICS_HPP_
#define _STATISTICS_HPP_

#include "Weapon.hpp"

enum EMissionState {
  MS_ACCOMPLISHED,
  MS_FAILED
};
struct SStatistics {
  SStatistics() 
    : eMissionState(MS_FAILED),
      fTime(0),
      fHitpoints(0),
      fLostHitpoints(0),
      fUsedManapoints(0) {
    for (auto &ic : uiUsedItems) {
      ic = 0;
    }
  }
  
  EMissionState eMissionState;

  float fTime;
  
  // player data
  float fHitpoints;
  float fManapoints;
  float fLostHitpoints;
  float fUsedManapoints;

  unsigned int uiUsedItems[Weapon::I_COUNT];
};

#endif
