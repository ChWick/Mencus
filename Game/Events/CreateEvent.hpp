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

#ifndef _CREATE_EVENT_HPP_
#define _CREATE_EVENT_HPP_

#include "Event.hpp"
#include "../Message.hpp"

class CCreateEvent : public CEvent {
private:
  enum EGroups {
    GR_ENEMY,
    GR_OBJECT,
  };

  static EGroups parseGroup(const std::string &s);
  static std::string toString(EGroups eGroup);

  enum EPosition {
    POS_ABSOLUTE,
    POS_RELATIVE,
  };

  static EPosition parsePosition(const std::string &s);
  static std::string toString(EPosition ePosition);

  std::string m_sObjID;
  EGroups m_eGroup;
  unsigned int m_uiType;
  EPosition m_ePosition;
  Ogre::Vector2 m_vPosition;
  
public:
  CCreateEvent(CMap &map, CEntity &owner);
  CCreateEvent(CMap &map, CEntity &owner, const tinyxml2::XMLElement *pElem);

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;

protected:
  void start_impl(); 
};

#endif
