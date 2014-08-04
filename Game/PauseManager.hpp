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

#ifndef PAUSE_MANAGER_HPP
#define PAUSE_MANAGER_HPP

#include <OgreSingleton.h>
#include <list>
#include <map>

class CPauseCaller;
class CPauseListener;

enum EPauseTypes {
  PAUSE_NONE              = 0,
  PAUSE_PLAYER_INPUT      = 1,

  PAUSE_PLAYER_MOVEMENT   = 2,
  PAUSE_ENEMY_MOVEMENT    = 4,
  PAUSE_SHOT_MOVEMENT     = 8,
  PAUSE_GLOABL_MOVEMENT   = 14,

  PAUSE_VIDEO             = 128,


  PAUSE_MAP_UPDATE        = 256,
  PAUSE_MAP_RENDER        = 512,
  PAUSE_MAP               = 768,

  PAUSE_SCREENPLAY        = 1024,

  PAUSE_MAP_EDITOR        = 2048,

  PAUSE_ALL               = 4095,
};

class CPauseManager : public Ogre::Singleton<CPauseManager> {
private:
  std::list<CPauseCaller *> m_lPauseCallers;
  std::list<CPauseListener *> m_lPauseListeners;
  std::map<EPauseTypes, void (CPauseListener::*)(bool)> m_mapPauseListener;

  unsigned int m_uiOldPauseFlags;
public:
  static CPauseManager& getSingleton(void);
  static CPauseManager* getSingletonPtr(void);

  CPauseManager();

  void update();

  void addCaller(CPauseCaller *pCaller) {m_lPauseCallers.push_back(pCaller);}
  void removeCaller(CPauseCaller *pCaller) {m_lPauseCallers.remove(pCaller);}

  void addListener(CPauseListener *pListener);
  void removeListener(CPauseListener *pListener) {m_lPauseListeners.remove(pListener);}

  bool isPause(unsigned int uiFlags) {return (m_uiOldPauseFlags & uiFlags) == uiFlags;}
};

#endif // PAUSE_MANAGER_HPP
