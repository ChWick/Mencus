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

#include "EventCreator.hpp"
#include "ChangeTileEvent.hpp"
#include "MessageEvent.hpp"
#include "ToggleEvent.hpp"
#include "InjectMessageEvent.hpp"
#include "CreateEvent.hpp"

#include "XMLHelper.hpp"
#include "OgreException.h"
#include <iostream>
using namespace std;

using namespace XMLHelper;

CEvent *CEventCreator::create(CMap &map, CEntity &owner, const tinyxml2::XMLElement *pElem) {
  cout << "Event: " << Attribute(pElem, "type", "unknown", true) << endl;
  CEvent::ETypes eType = CEvent::parseEventType(Attribute(pElem, "type", "unknown", true));
  switch (eType) {
  case CEvent::EVENT_CHANGE_TILE:
    return new CChangeTileEvent(map, owner, pElem);
  case CEvent::EVENT_TOGGLE:
    return new CToggleEvent(map, owner, pElem);
  case CEvent::EVENT_MESSAGE:
    return new CMessageEvent(map, owner, pElem);
  case CEvent::EVENT_INJECT_MESSAGE:
    return new CInjectMessageEvent(map, owner, pElem);
  case CEvent::EVENT_CREATE:
    return new CCreateEvent(map, owner, pElem);
  default:
    throw Ogre::Exception(0, "Event type not implemented", __FILE__);
  }
}
