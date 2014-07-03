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

#ifndef _ACHIEVEMENTS_HPP_
#define _ACHIEVEMENTS_HPP_

#include <vector>

namespace SocialGaming {
  enum EAchievements {
    DEBUG_ACHIEVEMENT = 0,
    BOOTS_1,
    BOOTS_2,
    BOOTS_3,
    BOOTS_4,
    BOOTS_5,
    BOOTS_6,

    ACHIEVEMENT_COUNT,
  };
  struct SAchievementData {
    EAchievements id;
    int pointValue;
    int progress;// in percent
    bool unlocked;

    SAchievementData() 
      : id(DEBUG_ACHIEVEMENT),
	pointValue(0),
	progress(0),
	unlocked(false) {
    }
  };

  class SAchievementList : public std::vector<SAchievementData> {
  public:
    SAchievementList()
      : std::vector<SAchievementData>(ACHIEVEMENT_COUNT) {
    }

    int getUnlockedPointValueSum() const {
      int sum = 0;
      for (const SAchievementData &data : *this) {
	if (data.unlocked) {
	  sum += data.pointValue;
	}
     }
      
      return sum;
    }
    long long getProgessedPointValueSum() const {
      long long sum = 0;
      for (const SAchievementData &data : *this) {
        sum += data.pointValue * data.progress;
      }

      sum /= 100; // since progess is in percent
      
      return sum;
    }
  };
};

#endif
