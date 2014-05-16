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

#ifndef EXPLOSION_HPP
#define EXPLOSION_HPP

#include "AnimatedSprite.hpp"

class CMap;

class CExplosion : public CAnimatedSprite {
public:
  enum EExplosionTypes {
    ET_BOLT,
    ET_BOMB,
    ET_GREEN_MONSTER,
    ET_KNIGHT,
    ET_BEAR,
    ET_GHOST,
    ET_DARK_MAGICAN,
    ET_SANTA,
    ET_SKULL,

    ET_COUNT,
  };
private:
  Ogre::Real m_fFadingTimer;                 //!< after the last image the fading timer (alpha blending)
public:
  CExplosion(CMap &map, const Ogre::Vector2 &vCenter, EExplosionTypes eExplosionType);

  void update(Ogre::Real tpf);

  void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText);
};

#endif
