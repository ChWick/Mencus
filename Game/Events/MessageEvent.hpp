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
