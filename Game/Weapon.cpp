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

#include "Weapon.hpp"

namespace Weapon {
  CEGUI::String getPicture(unsigned int uiWeapon) {
    if (uiWeapon == Weapon::W_BOLT) {
      return "hud_weapons/bolt";
    }
    else if (uiWeapon == Weapon::W_BOMB) {
      return "hud_weapons/bomb";
    }
    else if (uiWeapon == Weapon::W_SHIELD) {
      return "hud_weapons/shield";
    }
    else if (uiWeapon == Weapon::W_COLUMN) {
      return "hud_weapons/column";
    }
    else if (uiWeapon == Weapon::I_HEALTH_POTION) {
      return "hud_weapons/health_potion";
    }
    else if (uiWeapon == Weapon::I_MANA_POTION) {
      return "hud_weapons/mana_potion";
    }
    else if (uiWeapon == Weapon::I_KEY) {
      return "hud_weapons/keys";
    }
    return "hud_weapons/UNDEFINED_WEAPON";
  }
  std::string toString(unsigned int uiWeapon) {
    switch (uiWeapon) {
    case I_BOLT:
      return "bolt";
    case I_BOMB:
      return "bomb";
    case I_SHIELD:
      return "shield";
    case I_COLUMN:
      return "column";
    case I_HEALTH_POTION:
      return "health_potion";
    case I_MANA_POTION:
      return "mana_potion";
    case I_KEY:
      return "keys";
    default:
      return "unknwon";
    }
  }
  EItems parseItem(const std::string &s) {
    if (s == "bolt") {return I_BOLT;}
    else if (s == "bomb") {return I_BOMB;}
    else if (s == "shield") {return I_SHIELD;}
    else if (s == "column") {return I_COLUMN;}
    else if (s == "health_potion") {return I_HEALTH_POTION;}
    else if (s == "mana_potion") {return I_MANA_POTION;}
    else if (s == "keys") {return I_KEY;}

    return I_COUNT;
  }
};
