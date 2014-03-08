#ifndef _WEAPON_HPP_
#define _WEAPON_HPP_

#include <CEGUI/String.h>

namespace Weapon {
  enum EWeapons {
    W_BOLT,
    W_BOMB,
    W_SHIELD,
    W_COLUMN,
    W_COUNT,
  };

  CEGUI::String getPicture(unsigned int uiWeapon);
};

#endif
