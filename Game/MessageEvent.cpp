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
  if (m_sText.length() > 0) {m_vPagesText.push_back(m_sText);}

  for (const tinyxml2::XMLElement *pPage = pElem->FirstChildElement();
       pPage;
       pPage = pPage->NextSiblingElement()) {
    m_vPagesText.push_back(Attribute(pPage, "text"));
  }
}
CMessageEvent::~CMessageEvent() {
}
void CMessageEvent::init() {
}
void CMessageEvent::start() {
  m_pMessageBox = new CHUDMessageBox(m_sTitle.c_str(), m_vPagesText);  
}
void CMessageEvent::stop() {
}
void CMessageEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  SetAttribute(pElem, "text", m_sText);
  SetAttribute(pElem, "title", m_sTitle);

  tinyxml2::XMLDocument *pDoc(pElem->GetDocument());
  for (auto &s : m_vPagesText) {
    tinyxml2::XMLElement *pPage = pDoc->NewElement("page");
    pElem->InsertEndChild(pPage);
    pPage->SetAttribute("text", s.c_str());
  }
}
