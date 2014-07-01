#ifndef _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_
#define _SOCIAL_GAMING_CONNECTION_INTERFACE_HPP_

#include "Achievements.hpp"
#include <string>

namespace SocialGaming {
  class CSocialGamingConnectionInterface {
  private:
    bool m_bSignedIn;
    std::string m_sName;
    std::string m_sLogoImagesetLabel;
    
  public:
    CSocialGamingConnectionInterface(const std::string &sName = "Social Gaming",
				     const std::string &sLogo = "AmazonGameCircle") 
      : m_bSignedIn(false),
	m_sName(sName),
	m_sLogoImagesetLabel(sLogo) {
    }

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
    virtual void updateAchievementsProgress(EAchievements achievement, float fPercentComplete) {}
    virtual void showSocalGamingOverlay() {}
  };
};

#endif
