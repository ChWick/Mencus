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

#ifndef SAVE_STATE_MANAGER
#define SAVE_STATE_MANAGER

#include <tinyxml2.h>
#include <vector>
#include <list>
#include <time.h>
#include <OgreSingleton.h>

class CPlayer;

class CSaveState {
private:
  const unsigned int m_uiActID;
  const unsigned int m_uiSceneID;

  const float m_fPlayerHitpoints;
  const float m_fPlayerManapoints;

  const bool m_bAccessible;

  const tm m_tmTime;

public:
  CSaveState(unsigned int uiActID,
             unsigned int uiSceneID,
             float fHP,
             float fMP,
             bool bAccessible,
             tm tmTime)
             :
               m_uiActID(uiActID),
               m_uiSceneID(uiSceneID),
               m_fPlayerHitpoints(fHP),
               m_fPlayerManapoints(fMP),
               m_bAccessible(bAccessible),
               m_tmTime(tmTime) {
  }
  CSaveState(const CSaveState &s)
  :
    m_uiActID(s.m_uiActID),
    m_uiSceneID(s.m_uiSceneID),
    m_fPlayerHitpoints(s.m_fPlayerHitpoints),
    m_fPlayerManapoints(s.m_fPlayerManapoints),
    m_bAccessible(s.m_bAccessible),
    m_tmTime(s.m_tmTime) {
  }

  unsigned int getActID() const {return m_uiActID;}
  unsigned int getSceneID() const {return m_uiSceneID;}
  float getPlayerHP() const {return m_fPlayerHitpoints;}
  float getPlayerMP() const {return m_fPlayerManapoints;}
  bool isAccessible() const {return m_bAccessible;}
  const tm &getTime() const {return m_tmTime;}
};

class CSaveStateManager : public Ogre::Singleton<CSaveStateManager> {
private:
  std::list<CSaveState> m_vSaveStates;
public:
  static CSaveStateManager &getSingleton();
  static CSaveStateManager *getSingletonPtr();

  CSaveStateManager();
  ~CSaveStateManager();

  void clear() {m_vSaveStates.clear(); writeXMLFile();}

  void write(unsigned int uiAct, unsigned int uiScene, CPlayer *pPlayer);
  const CSaveState *read(unsigned int uiAct, unsigned int uiScene) const;
  bool hasSaveState(unsigned int uiAct, unsigned int uiScene) const;
  void eraseSaveState(unsigned int uiAct, unsigned int uiScene);

  const std::list<CSaveState> &listSaveState() const {return m_vSaveStates;}
  const CSaveState *getLastSaveState() const {if (m_vSaveStates.size() > 0) {return &m_vSaveStates.back();} return NULL;}

  void writeXMLFile();
private:
};

#endif // SAFE_STATE_MANAGER
