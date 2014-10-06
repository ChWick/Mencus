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

#include "EventEmitter.hpp"
#include <OgreException.h>
#include "XMLHelper.hpp"
#include "Map.hpp"
#include <OgreLogManager.h>

using namespace XMLHelper;
using namespace EventEmitter;

namespace EventEmitter {

std::string toString(EEmitter eEmitter) {
  switch (eEmitter) {
  case EMIT_ON_CREATE:
    return "create";
  case EMIT_ON_DESTROY:
    return "destroy";
  case EMIT_ON_USER:
    return "user";
  case EMIT_ON_COLLISION:
    return "collision";
  case EMIT_ON_MESSAGE_BOX_PAGE_CHANGE:
    return "message_page_changed";
  case EMIT_ON_MESSAGE:
    return "message";
  }

  throw Ogre::Exception(0, "Emitter type " + Ogre::StringConverter::toString(eEmitter) + " could not be converted to a string", __FILE__);
}
EEmitter parseEmitter(const std::string &sString) {
  if (sString == "create") {return EMIT_ON_CREATE;}
  else if (sString == "destroy") {return EMIT_ON_DESTROY;}
  else if (sString == "user") {return EMIT_ON_USER;}
  else if (sString == "collision") {return EMIT_ON_COLLISION;}
  else if (sString == "message_page_changed") {return EMIT_ON_MESSAGE_BOX_PAGE_CHANGE;}
  else if (sString == "message") {return EMIT_ON_MESSAGE;}

  throw Ogre::Exception(0, "Emitter type " + sString + " could not be converted to a string", __FILE__);
}


std::string toString(ERepeatType eRepeatType) {
  switch (eRepeatType) {
  case REPEAT_NONE:
    return "none";
  case REPEAT_INFINITE:
    return "infinite";
  }

  throw Ogre::Exception(0, "Repeat type " + Ogre::StringConverter::toString(eRepeatType) + " could not be converted to a string", __FILE__);
}
ERepeatType parseRepeatType(const std::string &sString) {
  if (sString == "none") {
    return REPEAT_NONE;
  }
  else if (sString == "infinite") {
    return REPEAT_INFINITE;
  }


  throw Ogre::Exception(0, "Repeat type '" + sString + "' could not be converted to a string", __FILE__);
}

void COnCollision::init(const CMap &map) {
  m_pEntity = map.getChildRecursive(m_sSrcID);
  if (!m_pEntity) {
    throw Ogre::Exception(0, "An entity with id " + m_sSrcID + " was not found.", __FILE__);
  }
  else {
    Ogre::LogManager::getSingleton().logMessage("Entity with id " + m_sSrcID + " set as collisions object.");
  }
}

CEmitter *CCreator::create(const tinyxml2::XMLElement *pElem) {
  switch (parseEmitter(Attribute(pElem, "emitter"))) {
  case EMIT_ON_CREATE:
    return new COnCreate(pElem);
  case EMIT_ON_DESTROY:
    return new COnDestroy(pElem);
  case EMIT_ON_USER:
    return new COnUser(pElem);
  case EMIT_ON_COLLISION:
    return new COnCollision(pElem);
  case EMIT_ON_MESSAGE_BOX_PAGE_CHANGE:
    return new COnMessageBoxPageChange(pElem);
  case EMIT_ON_MESSAGE:
    return new COnMessage(pElem);
  default:
    throw Ogre::Exception(0, "Emitter could not be created", __FILE__);
  }
}

};
