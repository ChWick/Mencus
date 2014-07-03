#include "SocialGamingConnectionInterface.hpp"
#include "MessageHandler.hpp"
#include "MessageTypes.hpp"
#include "Assert.hpp"
#include "Log.hpp"

using namespace SocialGaming;

void CSocialGamingConnectionInterface::changeSignedIn(bool isSignedIn) {
  m_bSignedIn = isSignedIn;

  CMessageHandler::getSingleton().
    addMessage(CMessage(CMessage::MT_SOCIAL_GAMING).
	       setInt(SIGNED_IN_STATE_CHANGED).
	       setBool(isSignedIn));

  if (isSignedIn) {
    for (int i = 0; i < LEADERBOARDS_COUNT; i++) {
      updateLeaderboard(static_cast<ELeaderboards>(i), REASON_ALL);
    }
  }
}

void CSocialGamingConnectionInterface::updateLeaderboard(ELeaderboards leaderboard,
							 ELeaderboardUpdateReason reason) {

  if (reason & REASON_ACHIEVEMENT_UNLOCKED) {
    loadAchievements();
  }

  m_LeaderboardsToUpdate.remove(leaderboard);
  m_LeaderboardsToUpdate.push_back(leaderboard);
}

void CSocialGamingConnectionInterface::updateLeaderboardsList() {
  if (m_WaitingForLeaderboardUpdates != 0) {
    // still waiting for updates
    LOGV("Still waiting for updates (%d)", m_WaitingForLeaderboardUpdates);
    return;
  }

  for (ELeaderboards leaderboard : m_LeaderboardsToUpdate) {
    switch (leaderboard) {
    case GLOAL_RANKING:
      LOGV("Updating globsl ranking");
      updateLeaderboardValue(GLOAL_RANKING, m_Achievements.getProgessedPointValueSum());
      break;
    case LEADERBOARDS_COUNT:
      break;
    }
  }
}

void CSocialGamingConnectionInterface::onLeaderboadReasonUpdated(ELeaderboardUpdateReason eReason) {
  LOGV("Leaderboard reason updated (%d)", eReason);
  // removing flag
  
  m_WaitingForLeaderboardUpdates -= m_WaitingForLeaderboardUpdates & eReason;
  
  updateLeaderboardsList();
}
