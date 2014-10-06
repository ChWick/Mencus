#ifndef _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_
#define _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_

#include "Achievements.hpp"

namespace SocialGaming {
  class CSocialGamingConnectionInterface {
  private:
    bool m_bSignedIn;
  public:
    CSocialGamingConnectionInterface() 
      : m_bSignedIn(false) {
    }
    // access methods
    void changeSignedIn(bool isSignedIn);
    bool isSignedIn() const {return m_bSignedIn;}

    // virutal methods
    virtual void loadAchievements() {}
    virtual void updateAchievementsProgress(EAchievements achievement, float fPercentComplete) {}
    virtual void showSocalGamingOverlay() {}
  };
};

#endif
