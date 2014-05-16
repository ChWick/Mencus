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

#ifndef COLLISION_DEFINES_HPP
#define COLLISION_DEFINES_HPP

//! Directions in that a collision occured
enum ECollisionCheckDirections {
  CCD_NONE        = 0,
  CCD_TOP         = 1,
  CCD_LEFT        = 2,
  CCD_BOTTOM      = 4,
  CCD_RIGHT       = 8,

  CCD_HORIZONTAL  = 10,
  CCD_VERTICAL    = 5,
  CCD_ALL         = 15,
};
enum ECollisionLineDirections {
  CLD_HORIZONTAL,
  CLD_VERTICAL,
};

#endif
