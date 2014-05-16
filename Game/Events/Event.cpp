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

#include "Event.hpp"
#include "EventEmitter.hpp"
#include "OgreException.h"
#include "OgreStringConverter.h"
#include "IDGenerator.hpp"

using namespace XMLHelper;

std::string CEvent::toString(ETypes eEventType) {
  switch (eEventType) {
  case EVENT_CHANGE_TILE:
    return "change_tile";
  case EVENT_MESSAGE:
    return "message";
  case EVENT_TOGGLE:
    return "toggle";
  case EVENT_INJECT_MESSAGE:
    return "inject_message";
  }
  
  throw Ogre::Exception(0, "Event type " + Ogre::StringConverter::toString(eEventType) + " could not be converted to a string", __FILE__);
}
CEvent::ETypes CEvent::parseEventType(const std::string &sString) {
  if (sString == "change_tile") {return EVENT_CHANGE_TILE;}
  else if (sString == "message") {return EVENT_MESSAGE;}
  else if (sString == "toggle") {return EVENT_TOGGLE;}
  else if (sString == "inject_message") {return EVENT_INJECT_MESSAGE;}
  
  throw Ogre::Exception(0, "Event type " + sString + " could not be converted to a string", __FILE__);
}



CEvent::CEvent(CMap &map, ETypes eType)
  : m_eType(eType),
    m_pEmitter(new EventEmitter::COnUser(NULL)),
    m_sID(CIDGenerator::nextID("Event")),
    m_Map(map),
    m_bStarted(false) {
}
CEvent::CEvent(CMap &map, ETypes eType, const tinyxml2::XMLElement *pElement)
  : m_eType(eType),
    m_pEmitter(EventEmitter::CCreator::create(pElement)),
    m_sID(Attribute(pElement, "id", CIDGenerator::nextID("Event"))),
    m_Map(map),
    m_bStarted(BoolAttribute(pElement, "started", false)) {
}
CEvent::~CEvent() {
  delete m_pEmitter;
}
void CEvent::init() {
  m_pEmitter->init(m_Map);
}
void CEvent::start() {
  if (!m_bStarted) {
    m_bStarted = true;
    start_impl();
  }
}
void CEvent::stop() {
  if (m_bStarted) {
    m_bStarted = false;
    stop_impl();
  }
}
void CEvent::writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const {
  SetAttribute(pElement, "type", toString(m_eType));
  SetAttribute(pElement, "id", m_sID);
  if (eStyle == OS_FULL) {
    SetAttribute(pElement, "started", m_bStarted);
  }
  m_pEmitter->writeToXMLElement(pElement);
}
