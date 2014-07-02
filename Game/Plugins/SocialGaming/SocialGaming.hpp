#ifndef _SOCIAL_GAMING_HPP_
#define _SOCIAL_GAMING_HPP_

#include "Achievements.hpp"
#include <OgreSingleton.h>
#include "Statistics.hpp"

namespace SocialGaming {
  class CSocialGamingConnectionInterface;
  class CSocialGaming : public Ogre::Singleton<CSocialGaming> {
  private:
    CSocialGamingConnectionInterface *m_pConnection;

  public:
    static CSocialGaming &getSingleton() {return *msSingleton;}
    static CSocialGaming *getSingletonPtr() {return msSingleton;}

    CSocialGaming();

    void init();

    CSocialGamingConnectionInterface *getConnection() {return m_pConnection;}

    void update(const SStatistics &stats);

    void updateAchievementsProgress(EAchievements achievement, float fPercentComplete);
  };
};

#endif
