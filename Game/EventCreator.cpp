#include "EventCreator.hpp"
#include "ChangeTileEvent.hpp"
#include "XMLHelper.hpp"
#include "OgreException.h"

using namespace XMLHelper;

CEvent *CEventCreator::create(CMap &map, const tinyxml2::XMLElement *pElem) {
  CEvent::ETypes eType = CEvent::parseEventType(Attribute(pElem, "type", "unknown", true));
  switch (eType) {
  case CEvent::EVENT_CHANGE_TILE:
    return new CChangeTileEvent(map, pElem);
    break;
  default:
    throw Ogre::Exception(0, "Event type not implemented", __FILE__);
  }
}
