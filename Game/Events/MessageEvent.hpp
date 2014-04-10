#ifndef _MESSAGE_EVENT_HPP_
#define _MESSAGE_EVENT_HPP_

#include "Event.hpp"
#include <vector>

class CHUDMessageBox;

class CMessageEvent : public CEvent {
private:
  std::string m_sTitle;
  std::string m_sText;
  std::vector<std::string> m_vPagesText;

  CHUDMessageBox *m_pMessageBox;
public:
  CMessageEvent(CMap &map);
  CMessageEvent(CMap &map, const tinyxml2::XMLElement *pElem);
  ~CMessageEvent();

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;

protected:
  void start_impl();
  void stop_impl();
};

#endif
