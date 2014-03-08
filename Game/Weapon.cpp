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
    return "hud_weapons/UNDEFINED_WEAPON";
  }
};
