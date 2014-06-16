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

#include "CreateEvent.hpp"
#include "XMLHelper.hpp"
#include "Object.hpp"
#include "Assert.hpp"
#include "Map.hpp"

using namespace XMLHelper;


CCreateEvent::EGroups CCreateEvent::parseGroup(const std::string &s) {
  if (s == "enemy") {return GR_ENEMY;}
  else if (s == "object") {return GR_OBJECT;}

  throw Ogre::Exception(0, "'" + s + "' could not be parsed as a group type", __FILE__);
}
std::string CCreateEvent::toString(CCreateEvent::EGroups eGroup) {
  switch (eGroup) {
  case GR_ENEMY: return "enemy";
  case GR_OBJECT: return "object";
  }
  throw Ogre::Exception(0, "Conversion not implemented", __FILE__);
}
CCreateEvent::EPosition CCreateEvent::parsePosition(const std::string &s) {
  if (s == "absolute") {return POS_ABSOLUTE;}
  else if (s == "relative") {return POS_RELATIVE;}
  
  throw Ogre::Exception(0, "'" + s + "' could not be parsed as a position type", __FILE__);
}
std::string CCreateEvent::toString(EPosition ePosition) {
  switch (ePosition) {
  case POS_RELATIVE: return "relative";
  case POS_ABSOLUTE: return "absolute";
  }
  throw Ogre::Exception(0, "Conversion not implemented", __FILE__);
}
CCreateEvent::CCreateEvent(CMap &map, CEntity &owner) 
  : CEvent(map, owner, EVENT_CREATE) {
}
CCreateEvent::CCreateEvent(CMap &map, CEntity &owner, const tinyxml2::XMLElement *pElem) 
  : CEvent(map, owner, EVENT_CREATE, pElem),
    m_sObjID(Attribute(pElem, "obj_id")),
    m_eGroup(parseGroup(Attribute(pElem, "obj_group"))),
    m_uiType(IntAttribute(pElem, "obj_type")),
    m_ePosition(parsePosition(Attribute(pElem, "obj_pos"))),
    m_vPosition(Vector2Attribute(pElem, "obj_")) {
}
void CCreateEvent::start_impl() {
  Ogre::LogManager::getSingleton().logMessage("Creating an entity from event");
  CEntity *pEntity = nullptr;
  switch (m_eGroup) {
  case GR_OBJECT:
    pEntity = new CObject(m_Map, m_sObjID, m_Map.getObjectsEntity(), m_vPosition, static_cast<CObject::EObjectTypes>(m_uiType));
    break;
  case GR_ENEMY:
    break;
  }
  
  MENCUS_ASSERT(pEntity);
  
  pEntity->init();

  switch (m_ePosition) {
  case POS_ABSOLUTE:
    pEntity->setPosition(m_vPosition);
    break;
  case POS_RELATIVE:
    pEntity->setPosition(m_vPosition + m_Owner.getPosition());
    break;
  }
}
void CCreateEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  SetAttribute(pElem, "obj_id", m_sObjID);
  SetAttribute(pElem, "obj_group", toString(m_eGroup));
  SetAttribute(pElem, "obj_type", m_uiType);
  SetAttribute(pElem, "obj_pos", toString(m_ePosition));
  SetAttribute(pElem, "obj_", m_vPosition);
}

