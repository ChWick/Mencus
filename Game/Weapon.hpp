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

#ifndef _WEAPON_HPP_
#define _WEAPON_HPP_

#include <CEGUI/String.h>
#include <string>

namespace Weapon {
  enum EWeapons {
    W_BOLT,
    W_BOMB,
    W_SHIELD,
    W_COLUMN,
    W_COUNT,
  };
  enum EItems {
    I_BOLT,
    I_BOMB,
    I_SHIELD,
    I_COLUMN,
    I_MANA_POTION,
    I_HEALTH_POTION,
    I_KEY,
    I_COUNT,
  };

  CEGUI::String getPicture(unsigned int uiWeapon);

  std::string toString(unsigned int uiWeapon);
};

#endif
