/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

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

class CEntity;

class CEvent {
public:
  enum ETypes {
    EVENT_CHANGE_TILE,
    EVENT_TOGGLE,
    EVENT_MESSAGE,
    EVENT_INJECT_MESSAGE,
    EVENT_CREATE,
  };
  static std::string toString(ETypes eEventType);
  static ETypes parseEventType(const std::string &sString);
private:
  const ETypes m_eType;
  EventEmitter::CEmitter *m_pEmitter;
  const std::string m_sID;
protected:
  CMap &m_Map;
  CEntity &m_Owner;
  bool m_bStarted;

protected:
  CEvent(CMap &map, CEntity &owner, ETypes eType);
  CEvent(CMap &map, CEntity &owner, ETypes eType, const tinyxml2::XMLElement *pElement);
public:
  virtual ~CEvent();
  
  virtual void init();
  virtual void exit() {}
  
  void start();
  void stop();

  const std::string &getID() const {return m_sID;}
  ETypes getType() const {return m_eType;}
  const EventEmitter::CEmitter *getEmitter() const {return m_pEmitter;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;

protected:
  virtual void start_impl() {}
  virtual void stop_impl() {}
};

#endif
