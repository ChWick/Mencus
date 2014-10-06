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

#ifndef _WHISPERSYNC_GAME_DATA_HPP_
#define _WHISPERSYNC_GAME_DATA_HPP_

#include "GameData.hpp"
#include "GlobalBuildDefines.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1

namespace Whispersync {
  class CGameData
    : public SocialGaming::CGameData {
  public:
    SocialGaming::CLevelList getLevelList();
  };
};

#endif

#endif
