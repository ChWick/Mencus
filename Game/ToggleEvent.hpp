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

  void start();
  void stop();

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;
};

#endif
