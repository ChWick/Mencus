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

#include "InjectMessageEvent.hpp"
#include "XMLHelper.hpp"
#include "MessageHandler.hpp"

using namespace XMLHelper;


CInjectMessageEvent::CInjectMessageEvent(CMap &map, CEntity &owner) 
  : CEvent(map, owner, EVENT_INJECT_MESSAGE) {
}
CInjectMessageEvent::CInjectMessageEvent(CMap &map, CEntity &owner, const tinyxml2::XMLElement *pElem) 
  : CEvent(map, owner, EVENT_INJECT_MESSAGE, pElem) {
  m_Message.setType(CMessage::parseMessageType(Attribute(pElem, "msg_type")));

  switch (m_Message.getType()) {
  case CMessage::MT_MESSAGE_BOX_PAGE_CHANGED:
    m_Message
      .setID(Attribute(pElem, "msg_id"))
      .setInt(IntAttribute(pElem, "msg_page"));
    break;
  case CMessage::MT_TOGGLE_TOUCH_INPUT_BLINK:
    m_Message
      .setID(Attribute(pElem, "msg_button"))
      .setBool(BoolAttribute(pElem, "msg_enabled"));
    break;
  default:
    break;
  };
}
void CInjectMessageEvent::start_impl() {
  CMessageHandler::getSingleton().addMessage(m_Message);
  // and stop again, that it can be repeated
  stop();
}
void CInjectMessageEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  SetAttribute(pElem, "msg_type", CMessage::toString(static_cast<CMessage::EMessageTypes>(m_Message.getType())));

  switch (m_Message.getType()) {
  case CMessage::MT_MESSAGE_BOX_PAGE_CHANGED:
    SetAttribute(pElem, "msg_id", m_Message.getID());
    SetAttribute(pElem, "msg_page", m_Message.getInt());
    break;
  case CMessage::MT_TOGGLE_TOUCH_INPUT_BLINK:
    SetAttribute(pElem, "msg_button", m_Message.getID());
    SetAttribute(pElem, "msg_enabled", m_Message.getBool());
    break;
  default:
    break;
  };
}

