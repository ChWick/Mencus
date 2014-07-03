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

#ifndef _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_
#define _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_

#include "Achievements.hpp"
#include "Leaderboards.hpp"
#include <string>
#include <vector>
#include <list>

namespace SocialGaming {

  enum ELeaderboardUpdateReason {
    REASON_NONE = 0,
    REASON_ACHIEVEMENT_UNLOCKED = 1,

    REASON_ALL = 1023,
  };

  class CSocialGamingConnectionInterface {
  private:
    bool m_bSignedIn;
    std::string m_sName;
    std::string m_sLogoImagesetLabel;

    unsigned int m_WaitingForLeaderboardUpdates;
    std::list<ELeaderboards> m_LeaderboardsToUpdate;
  protected:
    SAchievementList m_Achievements;
  public:
    CSocialGamingConnectionInterface(const std::string &sName = "Social Gaming",
				     const std::string &sLogo = "AmazonGameCircle") 
      : m_bSignedIn(false),
	m_sName(sName),
	m_sLogoImagesetLabel(sLogo),
	m_WaitingForLeaderboardUpdates(REASON_NONE) {
    }

    // syncing methods
    void updateLeaderboard(ELeaderboards leaderboard,
			   ELeaderboardUpdateReason reason = REASON_ALL);

    // access methods
    void changeSignedIn(bool isSignedIn);
    bool isSignedIn() const {return m_bSignedIn;}
    const std::string &getName() const {return m_sName;}
    const std::string &getLogoImagesetLabel() const {return m_sLogoImagesetLabel;}

    // virutal methods
    // =================================================================

    //! function to call when initializing the social gaming (e.g. login)
    /*
     * call this function in a loop to wait until this function returns true
     * @return true if success
     */
    virtual bool init() {return true;}

    virtual void loadAchievements() {}
    virtual void updateLeaderboardValue(ELeaderboards leaderboard, long long llValue) {}
    virtual void updateAchievementsProgress(EAchievements achievement, float fPercentComplete) {}
    virtual void showSocalGamingOverlay() {}
    virtual void showSignInPage() {}


  protected:
    void updateLeaderboardsList();

    void onLeaderboadReasonUpdated(ELeaderboardUpdateReason eReason);
  };
};

#endif
