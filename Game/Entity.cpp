#include "Entity.hpp"
#include "XMLHelper.hpp"
#include "Event.hpp"
#include "EventCreator.hpp"

using namespace XMLHelper;


CEntity::CEntity(CMap &map, const std::string &sID, CEntity *pParent)
  : m_sID(sID),
    m_Map(map),
    m_pParent(pParent),
    
    m_vPosition(Ogre::Vector2::ZERO),
    m_vSize(Ogre::Vector2::ZERO),
    m_vScale(Ogre::Vector2::UNIT_SCALE) {
  attachTo(pParent);
}
CEntity::CEntity(const CEntity &src) 
  : m_sID(src.m_sID),
    m_Map(src.m_Map),
    m_pParent(src.m_pParent),

    m_vPosition(src.m_vPosition),
    m_vSize(src.m_vSize),
    m_vScale(src.m_vScale),
    m_bbRelativeBoundingBox(src.m_bbRelativeBoundingBox) {
  attachTo(src.m_pParent);
  // not copying children so far
  // and for events
  // so dont use it...
}
CEntity::CEntity(CEntity *pParent,
		 CMap &map,
		 const tinyxml2::XMLElement *pElem,
		 const Ogre::Vector2 &vDefaultPosition,
		 const Ogre::Vector2 &vDefaultSize,
		 const Ogre::Vector2 &vDefaultScale) 
  : m_sID(Attribute(pElem, "id", "")),
    m_Map(map),
    m_pParent(NULL),
    m_vPosition(Vector2Attribute(pElem, "", vDefaultPosition)),
    m_vSize(Vector2Attribute(pElem, "size", vDefaultSize)),
    m_vScale(Vector2Attribute(pElem, "scale", vDefaultScale)),
    m_bbRelativeBoundingBox(BoundingBox2dAttribute(pElem,
						   CBoundingBox2d(Ogre::Vector2::ZERO,
								  vDefaultSize * vDefaultScale),
						   "relative_")) {
  attachTo(pParent);

  // read events
  using namespace tinyxml2;
  const XMLElement *pEventsElement = pElem->FirstChildElement("events");
  if (pEventsElement) {
    for (const XMLElement *pEventElement = pEventsElement->FirstChildElement();
	 pEventElement;
	 pEventElement = pEventElement->NextSiblingElement()) {
      m_lEvents.push_back(CEventCreator::create(m_Map, pEventElement));
    }
  }
}
CEntity::~CEntity() {
  while (m_lChildren.size() > 0) {
    delete m_lChildren.front();
    m_lChildren.pop_front();
  }
  while (m_lEvents.size() > 0) {
    delete m_lEvents.front();
    m_lEvents.pop_front();
  }
}
void CEntity::init() {
  for (auto pEvent: m_lEvents) {
    pEvent->init();
  }
}
void CEntity::exit() {
  for (auto pEvent: m_lEvents) {
    pEvent->exit();
  }
}
void CEntity::attachTo(CEntity *pParent) {
  if (m_pParent) {
    m_pParent->m_lChildren.remove(this);
  }
  m_pParent = pParent; 

  if (m_pParent) {
    m_pParent->m_lChildren.push_back(this);
  }
}

void CEnemy::destroyEvent(CEvent *pEvent) {
  assert(pEvent);
  m_lEvents.remove(pEvent);
  delete pEvent;
}

void CEntity::update(Ogre::Real tpf) {
  for (auto &pEnt : m_lChildren) {
    pEnt->update(tpf);
  }
}
void CEntity::render(Ogre::Real tpf) {
  for (auto &pEnt : m_lChildren) {
    pEnt->render(tpf);
  }
}
void CEntity::writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const {
  using namespace tinyxml2;

  SetAttribute(pElement, "", m_vPosition); // pos without label
  SetAttribute(pElement, "id", m_sID);
  if (eStyle == OS_FULL) {
    SetAttribute(pElement, "size", m_vSize);
    SetAttribute(pElement, "scale", m_vScale);
    SetAttribute(pElement, m_bbRelativeBoundingBox, "relative_");
  }

  // write events
  XMLDocument *pDoc = pElement->GetDocument();
  XMLElement *pEventsElement = pDoc->NewElement("events");
  pElement->InsertEndChild(pEventsElement);

  for (auto pEvent : m_lEvents) {
    XMLElement *pEventElement = pDoc->NewElement("event");
    pEventsElement->InsertEndChild(pEventElement);
    pEvent->writeToXMLElement(pEventElement, eStyle);
  }
}
