#ifndef _EVENT_CREATOR_HPP_
#define _EVENT_CREATOR_HPP_

#include <tinyxml2.h>

class CEvent;
class CMap;

class CEventCreator {
public:
  static CEvent *create(CMap &map, const tinyxml2::XMLElement *pElem);
};

#endif
