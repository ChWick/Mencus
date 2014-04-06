#include "MessageEvent.hpp"
#include "XMLHelper.hpp"
#include "Map.hpp"
#include "OgreLogManager.h"
#include "HUDMessageBox.hpp"

using namespace XMLHelper;

CMessageEvent::CMessageEvent(CMap &map) 
  : CEvent(map, EVENT_MESSAGE),
    m_pMessageBox(NULL) {
}
CMessageEvent::CMessageEvent(CMap &map, const tinyxml2::XMLElement *pElem) 
  : CEvent(map, EVENT_MESSAGE, pElem),
    m_sTitle(Attribute(pElem,"title")),
    m_sText(Attribute(pElem, "text")),
    m_pMessageBox(NULL) {
}
CMessageEvent::~CMessageEvent() {
}
void CMessageEvent::init() {
}
void CMessageEvent::start() {
  m_pMessageBox = new CHUDMessageBox(m_sTitle.c_str(), m_sText.c_str());  
}
void CMessageEvent::stop() {
}
void CMessageEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  SetAttribute(pElem, "text", m_sText);
  SetAttribute(pElem, "title", m_sTitle);
}
