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

#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "Sprite.hpp"


class CBackground : public CSprite {
public:
  enum ETypes {
    BT_CLOUDS,
    BT_FOREST,
    BT_STARS,
    BT_FOG,

    BT_COUNT
  };

  static ETypes fromString(const Ogre::String &sName);
private:
  const ETypes m_eBackgroundType;
  const Ogre::Vector2 &m_vCameraPosition;
  const Ogre::String m_sName;
  const Ogre::Vector2 &m_vTilesPerScreen;
  const float &m_fScreenRatio;
public:
  CBackground(CMap &map,
	      const Ogre::Vector2 &vCameraPosition,
	      const Ogre::String &sName,
	      const Ogre::Vector2 &vTilesPerScreen,
	      const float &m_fScreenRatio);

  const Ogre::String &getName() const {return m_sName;}

  virtual void render(Ogre::Real tpf);
};

#endif // BACKGROUND_HPP
