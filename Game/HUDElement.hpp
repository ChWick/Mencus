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

#ifndef _HUD_ELEMENT_HPP_
#define _HUD_ELEMENT_HPP_

#include "HUD.hpp"

class CHUDElement {
public:
  CHUDElement() {
    CHUD::getSingleton().getElements().push_back(this);
  }
  virtual ~CHUDElement() {
    CHUD::getSingleton().getElements().remove(this);
  }
};

#endif
