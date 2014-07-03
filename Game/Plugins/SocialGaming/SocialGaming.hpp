#ifndef _SOCIAL_GAMING_HPP_
#define _SOCIAL_GAMING_HPP_

#include "Achievements.hpp"
#include <OgreSingleton.h>
#include "Statistics.hpp"

namespace SocialGaming {
  enum EConnectionStatus {
    CONNECTED,
    CONNECTING,
    DISCONNECTED
  };
  class CSocialGamingConnectionInterface;

  class CSocialGaming : public Ogre::Singleton<CSocialGaming> {
  private:
    CSocialGamingConnectionInterface *m_pConnection;

    EConnectionStatus m_eConnectionStatus;
    float m_fConnectionCheckTimer;
  public:
    static CSocialGaming &getSingleton() {return *msSingleton;}
    static CSocialGaming *getSingletonPtr() {return msSingleton;}

    CSocialGaming();

    void init();
    void update(float tpf);

    CSocialGamingConnectionInterface *getConnection() {return m_pConnection;}
    EConnectionStatus getConnectionStatus() {return m_eConnectionStatus;}

    void update(const SStatistics &stats);

    void updateAchievementsProgress(EAchievements achievement, float fPercentComplete);
  };
};

#endif
