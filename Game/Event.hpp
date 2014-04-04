#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include <string>
#include <tinyxml2.h>
#include "XMLHelper.hpp"
#include "OutputStyle.hpp"

class CMap;

class CEvent {
public:
  enum ETypes {
    EVENT_CHANGE_TILE,
    EVENT_SHOW_MESSAGE,
  };
  static std::string toString(ETypes eEventType);
  static ETypes parseEventType(const std::string &sString);
private:
  const ETypes m_eType;
  CMap &m_Map;

protected:
  CEvent(CMap &map, ETypes eType);
public:
  virtual ~CEvent();
  
  virtual void init() {}
  virtual void exit() {}

  ETypes getType() const {return m_eType;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;
};

#endif
