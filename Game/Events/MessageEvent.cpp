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

#include "MessageEvent.hpp"
#include "XMLHelper.hpp"
#include "Map.hpp"
#include "OgreLogManager.h"
#include "HUDMessageBox.hpp"
#include "MapPack.hpp"

using namespace XMLHelper;

CMessageEvent::CMessageEvent(CMap &map) 
  : CEvent(map, EVENT_MESSAGE),
    m_pMessageBox(NULL) {
}
CMessageEvent::CMessageEvent(CMap &map, const tinyxml2::XMLElement *pElem) 
  : CEvent(map, EVENT_MESSAGE, pElem),
    m_sTitle(Attribute(pElem,"title")),
    m_sText(Attribute(pElem, "text", "")),
    m_pMessageBox(NULL) {
  if (m_sText.length() > 0) {m_vPagesText.push_back(m_sText);}

  for (const tinyxml2::XMLElement *pPage = pElem->FirstChildElement();
       pPage;
       pPage = pPage->NextSiblingElement()) {
    m_vPagesText.push_back(Attribute(pPage, "text"));
  }

  assert(m_vPagesText.size() > 0);
}
CMessageEvent::~CMessageEvent() {
}
void CMessageEvent::start_impl() {
  // read translation strings
  std::vector<std::string> vTranslatedPagesText(m_vPagesText.size());
  for (unsigned int i = 0; i < m_vPagesText.size(); i++){
    vTranslatedPagesText[i] = m_Map.getMapPack()->getString(m_vPagesText[i]);
  }
  m_pMessageBox = new CHUDMessageBox(getID(), m_Map.getMapPack()->getCEGUIString(m_sTitle), vTranslatedPagesText);
}
void CMessageEvent::stop_impl() {
}
void CMessageEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  SetAttribute(pElem, "title", m_sTitle);

  tinyxml2::XMLDocument *pDoc(pElem->GetDocument());
  for (auto &s : m_vPagesText) {
    tinyxml2::XMLElement *pPage = pDoc->NewElement("page");
    pElem->InsertEndChild(pPage);
    pPage->SetAttribute("text", s.c_str());
  }
}
