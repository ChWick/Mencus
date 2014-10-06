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

#ifndef _ACHIEVEMENT_PROGRESS_TESTER_HPP_
#define _ACHIEVEMENT_PROGRESS_TESTER_HPP_

#include "Achievements.hpp"
#include "Statistics.hpp"

namespace SocialGaming {

  class CSocialGaming;

  class CAchievementProgressTester {
  private:
    CSocialGaming &m_SocialGaming;
  public:
    CAchievementProgressTester(CSocialGaming &socialGaming) 
      : m_SocialGaming(socialGaming) {
    }
    
    void update(const SStatistics &stats);

  private:
    void updateAchievementsProgress(EAchievements achievement, float fPercentComplete);
  };
};

#endif
