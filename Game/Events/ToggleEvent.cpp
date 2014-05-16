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

#include "ToggleEvent.hpp"
#include "XMLHelper.hpp"
#include "Map.hpp"
#include "OgreLogManager.h"

using namespace XMLHelper;

CToggleEvent::CToggleEvent(CMap &map) 
  : CEvent(map, EVENT_TOGGLE),
    m_sEntityID(""),
    m_bInitialState(true) {
}
CToggleEvent::CToggleEvent(CMap &map, const tinyxml2::XMLElement *pElem) 
  : CEvent(map, EVENT_TOGGLE, pElem),
    m_sEntityID(Attribute(pElem, "id", "")),
    m_bInitialState(BoolAttribute(pElem, "initial", true)) {
}
void CToggleEvent::init() {
  CEvent::init();
  CEntity *pEnt = m_Map.getChildRecursive(m_sEntityID);
  if (pEnt) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Entity with id " + m_sEntityID + " was not found.");
    return;
  }
  pEnt->setActivated(m_bInitialState);
}
void CToggleEvent::start_impl() {
  m_Map.getChildRecursive(m_sEntityID)->setActivated(!m_bInitialState);
}
void CToggleEvent::stop_impl() {
  m_Map.getChildRecursive(m_sEntityID)->setActivated(m_bInitialState);
}
void CToggleEvent::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  CEvent::writeToXMLElement(pElem, eStyle);

  pElem->SetAttribute("id", m_sEntityID.c_str());
  SetAttribute(pElem, "initial", m_bInitialState);
}
