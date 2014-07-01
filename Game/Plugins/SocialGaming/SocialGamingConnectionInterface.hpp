#ifndef _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_
#define _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_

#include "Achievements.hpp"
#include <string>

namespace SocialGaming {
  class CSocialGamingConnectionInterface {
  private:
    bool m_bSignedIn;
    std::string m_sName;
    
  public:
    CSocialGamingConnectionInterface(const std::string &sName = "Social Gaming") 
      : m_bSignedIn(false),
	m_sName(sName) {
    }

    // access methods
    void changeSignedIn(bool isSignedIn);
    bool isSignedIn() const {return m_bSignedIn;}
    const std::string &getName() const {return m_sName;}

    // virutal methods
    // =================================================================

    //! function to call when initializing the social gaming (e.g. login)
    /*
     * call this function in a loop to wait until this function returns true
     * @return true if success
     */
    virtual bool init() {return true;}

    virtual void loadAchievements() {}
    virtual void updateAchievementsProgress(EAchievements achievement, float fPercentComplete) {}
    virtual void showSocalGamingOverlay() {}
  };
};

#endif
