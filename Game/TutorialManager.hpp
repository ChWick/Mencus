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

#ifndef _TUTORIAL_MANAGER_HPP_
#define _TUTORIAL_MANAGER_HPP_

#include <vector>

class CMap;

class CTutorialManager {
  enum EInformationStates {
    IS_MOTION = 0,
    IS_SWITCH,
    IS_ENEMY,
    IS_SMALL_ENEMY,

    IS_COUNT,
  };
private:
  bool m_bEnabled;
  const CMap &m_Map;
  std::vector<bool> m_bInformationShown;
public:
  CTutorialManager(const CMap &map);

  void setEnabled(bool bEnabled) {m_bEnabled = bEnabled;}

  void start();

  void update();
};

#endif
