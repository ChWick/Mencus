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
