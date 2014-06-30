#ifndef _SOCIAL_GAMING_HPP_
#define _SOCIAL_GAMING_HPP_

#include "Achievements.hpp"
#include <OgreSingleton.h>
#include "Statistics.hpp"

namespace SocialGaming {
  class CSocialGamingConnectionInterface;
  class CSocialGaming : public Ogre::Singleton<CSocialGaming> {
  private:

    float m_afAchievements[ACHIEVEMENT_COUNT];
    CSocialGamingConnectionInterface *m_pConnection;

  public:
    static CSocialGaming &getSingleton() {return *msSingleton;}
    static CSocialGaming *getSingletonPtr() {return msSingleton;}

    CSocialGaming();

    CSocialGamingConnectionInterface *getConnection() {return m_pConnection;}

    void update(const SStatistics &stats);

    void updateAchievementsProgress(EAchievements achievement, float fPercentComplete);
  };
};

#endif
