#include "EventCreator.hpp"
#include "ChangeTileEvent.hpp"
#include "MessageEvent.hpp"
#include "ToggleEvent.hpp"
#include "InjectMessageEvent.hpp"

#include "XMLHelper.hpp"
#include "OgreException.h"
#include <iostream>
using namespace std;

using namespace XMLHelper;

CEvent *CEventCreator::create(CMap &map, const tinyxml2::XMLElement *pElem) {
  cout << "Event:" << Attribute(pElem, "type", "unknown", true) << endl;
  CEvent::ETypes eType = CEvent::parseEventType(Attribute(pElem, "type", "unknown", true));
  switch (eType) {
  case CEvent::EVENT_CHANGE_TILE:
    return new CChangeTileEvent(map, pElem);
  case CEvent::EVENT_TOGGLE:
    return new CToggleEvent(map, pElem);
  case CEvent::EVENT_MESSAGE:
    return new CMessageEvent(map, pElem);
  case CEvent::EVENT_INJECT_MESSAGE:
    return new CInjectMessageEvent(map, pElem);
  default:
    throw Ogre::Exception(0, "Event type not implemented", __FILE__);
  }
}