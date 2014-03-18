#ifndef _STATISTICS_HPP_
#define _STATISTICS_HPP_

#include "Weapon.hpp"

struct SStatistics {
  SStatistics() 
    : fTime(0),
      fHitpoints(0),
      fLostHitpoints(0),
      fUsedManapoints(0) {
    for (auto &ic : uiUsedItems) {
      ic = 0;
    }
  }
  float fTime;
  
  // player data
  float fHitpoints;
  float fManapoints;
  float fLostHitpoints;
  float fUsedManapoints;

  unsigned int uiUsedItems[Weapon::I_COUNT];
};

#endif
