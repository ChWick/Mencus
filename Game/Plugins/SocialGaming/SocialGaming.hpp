#ifndef _SOCIAL_GAMING_HPP_
#define _SOCIAL_GAMING_HPP_

#include "Achievements.hpp"
namespace SocialGaming {
  class CSocialGamingConnectionInterface;
  class CSocialGaming {
  private:
    static CSocialGaming m_Singleton;

    bool m_abAchievements[ACHIEVEMENT_COUNT];
    CSocialGamingConnectionInterface *m_pConnection;
  
    
  private:
    CSocialGaming();

  public:
    static CSocialGaming &getSingleton() {return m_Singleton;}
    static CSocialGaming *getSingletonPtr() {return &m_Singleton;}
  };
};

#endif
