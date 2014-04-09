#include "Event.hpp"
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
  }
  
  throw Ogre::Exception(0, "Event type " + Ogre::StringConverter::toString(eEventType) + " could not be converted to a string", __FILE__);
}
CEvent::ETypes CEvent::parseEventType(const std::string &sString) {
  if (sString == "change_tile") {return EVENT_CHANGE_TILE;}
  else if (sString == "message") {return EVENT_MESSAGE;}
  else if (sString == "toggle") {return EVENT_TOGGLE;}
  
  throw Ogre::Exception(0, "Event type " + sString + " could not be converted to a string", __FILE__);
}

std::string CEvent::toString(EEmitter eEmitter) {
  switch (eEmitter) {
  case EMIT_ON_CREATE:
    return "create";
  case EMIT_ON_DESTROY:
    return "destroy";
  case EMIT_ON_USER:
    return "user";
  case EMIT_ON_MESSAGE_BOX_PAGE_CHANGE:
    return "message_page_chagned";
  }
  
  throw Ogre::Exception(0, "Emitter type " + Ogre::StringConverter::toString(eEmitter) + " could not be converted to a string", __FILE__);
}
CEvent::EEmitter CEvent::parseEmitter(const std::string &sString) {
  if (sString == "create") {return EMIT_ON_CREATE;}
  else if (sString == "destroy") {return EMIT_ON_DESTROY;}
  else if (sString == "user") {return EMIT_ON_USER;}
  else if (sString == "message_page_chagned") {return EMIT_ON_MESSAGE_BOX_PAGE_CHANGE;}

  throw Ogre::Exception(0, "Emitter type " + sString + " could not be converted to a string", __FILE__);
}

CEvent::CEvent(CMap &map, ETypes eType)
  : m_eType(eType),
    m_eEmitter(EMIT_ON_USER),
    m_sID(CIDGenerator::nextID("Event")),
    m_Map(map) {
}
CEvent::CEvent(CMap &map, ETypes eType, const tinyxml2::XMLElement *pElement)
  : m_eType(eType),
    m_eEmitter(parseEmitter(Attribute(pElement, "emitter", "user"))),
    m_sID(Attribute(pElement, "id", CIDGenerator::nextID("Event"))),
    m_Map(map) {
}
CEvent::~CEvent() {
}
void CEvent::writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const {
  SetAttribute(pElement, "type", toString(m_eType));
  SetAttribute(pElement, "emitter", toString(m_eEmitter));
  SetAttribute(pElement, "id", m_sID);
}
