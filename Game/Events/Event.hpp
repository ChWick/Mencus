#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include <string>
#include <tinyxml2.h>
#include "XMLHelper.hpp"
#include "OutputStyle.hpp"

class CMap;
namespace EventEmitter {
  class CEmitter;
};

class CEvent {
public:
  enum ETypes {
    EVENT_CHANGE_TILE,
    EVENT_TOGGLE,
    EVENT_MESSAGE,
    EVENT_INJECT_MESSAGE,
  };
  static std::string toString(ETypes eEventType);
  static ETypes parseEventType(const std::string &sString);
private:
  const ETypes m_eType;
  const EventEmitter::CEmitter *m_pEmitter;
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

  const std::string &getID() const {return m_sID;}
  ETypes getType() const {return m_eType;}
  const EventEmitter::CEmitter *getEmitter() const {return m_pEmitter;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;
};

#endif
