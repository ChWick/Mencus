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

#ifndef _SOCIAL_GAMING_HPP_
#define _SOCIAL_GAMING_HPP_

#include "Achievements.hpp"
#include <OgreSingleton.h>
#include "Statistics.hpp"
#include "MessageInjector.hpp"

namespace SocialGaming {
  enum EConnectionStatus {
    CONNECTED,
    CONNECTING,
    DISCONNECTED
  };
  class CSocialGamingConnectionInterface;
  class CGameData;

  class CSocialGaming
    : public Ogre::Singleton<CSocialGaming>,
      public CMessageInjector {
  private:
    CSocialGamingConnectionInterface *m_pConnection;
    CGameData *m_pGameData;

    EConnectionStatus m_eConnectionStatus;
    float m_fConnectionCheckTimer;
  public:
    static CSocialGaming &getSingleton() {return *msSingleton;}
    static CSocialGaming *getSingletonPtr() {return msSingleton;}

    CSocialGaming();
    virtual ~CSocialGaming();

    void init();
    void update(float tpf);

    CSocialGamingConnectionInterface *getConnection() {return m_pConnection;}
    CGameData *getGameData() {return m_pGameData;}
    EConnectionStatus getConnectionStatus() {return m_eConnectionStatus;}

    void update(const SStatistics &stats);

    void updateAchievementsProgress(EAchievements achievement, float fPercentComplete);

    void sendMessageToAll(const CMessage &message);
  };
};

#endif
