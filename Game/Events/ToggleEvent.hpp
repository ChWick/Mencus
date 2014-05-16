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

#ifndef _TOGGLE_EVENT_HPP_
#define _TOGGLE_EVENT_HPP_

#include "Event.hpp"

class CToggleEvent : public CEvent {
private:
  Ogre::String m_sEntityID;
  bool m_bInitialState;
public:
  CToggleEvent(CMap &map);
  CToggleEvent(CMap &map, const tinyxml2::XMLElement *pElem);

  void init();

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;

protected:
  void start_impl();
  void stop_impl();
};

#endif
