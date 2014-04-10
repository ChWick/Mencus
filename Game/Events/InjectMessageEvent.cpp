#include "InjectMessageEvent.hpp"
#include "XMLHelper.hpp"
#include "MessageHandler.hpp"

using namespace XMLHelper;


CInjectMessageEvent::CInjectMessageEvent(CMap &map) 
  : CEvent(map, EVENT_INJECT_MESSAGE) {
}
CInjectMessageEvent::CInjectMessageEvent(CMap &map, const tinyxml2::XMLElement *pElem) 
  : CEvent(map, EVENT_INJECT_MESSAGE, pElem) {
  m_Message.setType(CMessage::parseMessageType(Attribute(pElem, "msg_type")));

  switch (m_Message.getType()) {
  case CMessage::MT_MESSAGE_UNSET:
    break;
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
  };
}
void CInjectMessageEvent::start_impl() {
  CMessageHandler::getSingleton().addMessage(m_Message);
}
void CInjectMessageEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  SetAttribute(pElem, "msg_type", CMessage::toString(static_cast<CMessage::EMessageTypes>(m_Message.getType())));

  switch (m_Message.getType()) {
  case CMessage::MT_MESSAGE_UNSET:
    break;
  case CMessage::MT_MESSAGE_BOX_PAGE_CHANGED:
    SetAttribute(pElem, "msg_id", m_Message.getID());
    SetAttribute(pElem, "msg_page", m_Message.getInt());
    break;
  case CMessage::MT_TOGGLE_TOUCH_INPUT_BLINK:
    SetAttribute(pElem, "msg_button", m_Message.getID());
    SetAttribute(pElem, "msg_enabled", m_Message.getBool());
    break;
  };
}

