#ifndef _MESSAGE_EVENT_HPP_
#define _MESSAGE_EVENT_HPP_

#include "Event.hpp"

class CHUDMessageBox;

class CMessageEvent : public CEvent {
private:
  std::string m_sTitle;
  std::string m_sText;

  CHUDMessageBox *m_pMessageBox;
public:
  CMessageEvent(CMap &map);
  CMessageEvent(CMap &map, const tinyxml2::XMLElement *pElem);
  ~CMessageEvent();

  void init();

  void start();
  void stop();

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;
};

#endif
