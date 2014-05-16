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

#include "AdDisplayManager.hpp"
#include "Game.hpp"
#include <OgreLogManager.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android/Android.hpp"
#endif

int CAdDisplayManager::m_iWaitingLoops(20);
int CAdDisplayManager::m_iWaitedLoops(0);

void CAdDisplayManager::showAdPopup() {
  m_iWaitedLoops = 0;
  CGame::getSingleton().showLoadingBar(0, 1);
  CGame::getSingleton().getTrayMgr()->resourceGroupLoadStarted("Please wait", m_iWaitingLoops);
  CGame::getSingleton().getTrayMgr()->worldGeometryStageStarted("Please wait");
#ifdef ADDS_ANDROID
  m_iWaitingLoops = 20; // 20 * 0.5 secs max waiting time for the add
  OgreAndroidBridge::showAdPopup();
#else
  m_iWaitingLoops = 0; // set to value > 0 for testing
#endif
}
bool CAdDisplayManager::adPopupClosed() {
  if (isAdPopupClosed()) {
    CGame::getSingleton().hideLoadingBar();
    return true;
  }
  return false;
}
bool CAdDisplayManager::isAdPopupClosed() {
  // to increase loading bar
  CGame::getSingleton().getTrayMgr()->resourceLoadEnded();
  m_iWaitedLoops += 1;
#ifdef ADDS_ANDROID
  if (m_iWaitedLoops > m_iWaitingLoops) {
    // force close of ad, not displaying
    OgreAndroidBridge::closeAd();
    return true;
  }
  return OgreAndroidBridge::adPopupClosed();
#else
  return m_iWaitedLoops > m_iWaitingLoops;
#endif
}
