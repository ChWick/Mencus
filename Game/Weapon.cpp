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
};
