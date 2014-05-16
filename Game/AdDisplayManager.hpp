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

#ifndef _AD_DISPLAY_MANAGER_HPP_
#define _AD_DISPLAY_MANAGER_HPP_

#include <OgrePlatform.h>
#include "GlobalBuildDefines.hpp"

#if MENCUS_ENABLE_ADS == 1

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#define ADDS_ANDROID
#endif

#endif

class CAdDisplayManager {
private:
  static int m_iWaitingLoops;
  static int m_iWaitedLoops;
public:
  static void setWaitingLoops(int iLoops) {m_iWaitingLoops = iLoops;}
  static void showAdPopup();
  static bool adPopupClosed();

private:
  static bool isAdPopupClosed();
};

#endif
