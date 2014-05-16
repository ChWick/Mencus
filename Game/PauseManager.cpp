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

#include "PauseManager.hpp"
#include "PauseListener.hpp"
#include "PauseCaller.hpp"

template<> CPauseManager* Ogre::Singleton<CPauseManager>::msSingleton = 0;

CPauseManager* CPauseManager::getSingletonPtr(void)
{
  return msSingleton;
}
CPauseManager& CPauseManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

CPauseManager::CPauseManager()
: m_uiOldPauseFlags(PAUSE_NONE) {

  m_mapPauseListener[PAUSE_PLAYER_INPUT] = &CPauseListener::playerInputPauseChanged;
  m_mapPauseListener[PAUSE_PLAYER_MOVEMENT] = &CPauseListener::playerMovePauseChanged;
  m_mapPauseListener[PAUSE_ENEMY_MOVEMENT] = &CPauseListener::enemyMovePauseChanged;
  m_mapPauseListener[PAUSE_VIDEO] = &CPauseListener::videoPauseChanged;
  m_mapPauseListener[PAUSE_MAP_UPDATE] = &CPauseListener::mapUpdatePauseChanged;
  m_mapPauseListener[PAUSE_MAP_RENDER] = &CPauseListener::mapRenderPauseChanged;
  m_mapPauseListener[PAUSE_SCREENPLAY] = &CPauseListener::screenplayPauseChanged;
  m_mapPauseListener[PAUSE_MAP_EDITOR] = &CPauseListener::mapEditorUpdatePauseChanged;
}

void CPauseManager::update() {
  unsigned int uiTotalPause = 0;
  for (auto pCaller : m_lPauseCallers) {
      uiTotalPause |= pCaller->getPause();
  }

  for (auto p : m_mapPauseListener) {
    if ((uiTotalPause & p.first) != (m_uiOldPauseFlags & p.first)) {
      for (auto pListener : m_lPauseListeners) {
        (pListener->*p.second)((uiTotalPause & p.first) > 0);
      }
    }
  }


  m_uiOldPauseFlags = uiTotalPause;
}
