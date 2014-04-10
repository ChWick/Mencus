#ifndef _INJECT_MESSAGE_EVENT_HPP_
#define _INJECT_MESSAGE_EVENT_HPP_

#include "Event.hpp"
#include "../Message.hpp"

class CInjectMessageEvent : public CEvent {
private:
  CMessage m_Message;
public:
  CInjectMessageEvent(CMap &map);
  CInjectMessageEvent(CMap &map, const tinyxml2::XMLElement *pElem);

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;

protected:
  void start_impl(); 
};

#endif
