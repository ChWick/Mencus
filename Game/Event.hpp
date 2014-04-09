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
    EVENT_TOGGLE,
    EVENT_MESSAGE,
  };
  enum EEmitter {
    EMIT_ON_CREATE,
    EMIT_ON_DESTROY,
    EMIT_ON_USER,
    EMIT_ON_MESSAGE_BOX_PAGE_CHANGE,
  };
  static std::string toString(ETypes eEventType);
  static ETypes parseEventType(const std::string &sString);

  static std::string toString(EEmitter eEmitter);
  static EEmitter parseEmitter(const std::string &sString);
private:
  const ETypes m_eType;
  const EEmitter m_eEmitter;
  const std::string m_sID;
protected:
  CMap &m_Map;

protected:
  CEvent(CMap &map, ETypes eType);
  CEvent(CMap &map, ETypes eType, const tinyxml2::XMLElement *pElement);
public:
  virtual ~CEvent();
  
  virtual void init() {}
  virtual void exit() {}
  
  virtual void start() {}
  virtual void stop() {}

  ETypes getType() const {return m_eType;}
  EEmitter getEmitter() const {return m_eEmitter;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;
};

#endif
