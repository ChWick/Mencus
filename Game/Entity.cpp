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

#include "Entity.hpp"
#include "XMLHelper.hpp"
#include "Events/Event.hpp"
#include "Events/EventCreator.hpp"
#include "IDGenerator.hpp"
#include "Map.hpp"
#include <OgreStringConverter.h>
#include "Message.hpp"
#include "Events/EventEmitter.hpp"

using namespace XMLHelper;


CEntity::CEntity(CMap &map, const std::string &sID, CEntity *pParent)
  : m_sID(sID.length() == 0 ? Ogre::StringConverter::toString(CIDGenerator::nextID()) : sID),
    m_uiType(0),
    m_bVisible(true),
    m_Map(map),
    m_pParent(pParent),
    
    m_vPosition(Ogre::Vector2::ZERO),
    m_vSize(Ogre::Vector2::ZERO),
    m_vScale(Ogre::Vector2::UNIT_SCALE) {
  attachTo(pParent);
}
CEntity::CEntity(const CEntity &src) 
  : m_sID(src.m_sID),
    m_uiType(src.m_uiType),
    m_bVisible(src.m_bVisible),
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
CEntity::CEntity(CMap &map,
		 CEntity *pParent,
		 const tinyxml2::XMLElement *pElem,
		 const Ogre::Vector2 &vDefaultPosition,
		 const Ogre::Vector2 &vDefaultSize,
		 const Ogre::Vector2 &vDefaultScale) 
  : m_sID(Ogre::StringConverter::toString(CIDGenerator::nextID())),
    m_uiType(0),
    m_bVisible(true),
    m_Map(map),
    m_pParent(NULL),
    m_vPosition(vDefaultPosition),
    m_vSize(vDefaultSize),
    m_vScale(vDefaultScale),
    m_bbRelativeBoundingBox(CBoundingBox2d(Ogre::Vector2::ZERO, vDefaultSize * vDefaultScale)) {

  attachTo(pParent);

  readFromXMLElement(pElem);
}
CEntity::~CEntity() {
  std::list<CEntity *> lClone(m_lChildren);
  m_lChildren.clear();
  while (lClone.size() > 0) {
    delete lClone.front();
    lClone.pop_front();
  }
  while (m_lEvents.size() > 0) {
    delete m_lEvents.front();
    m_lEvents.pop_front();
  }
  attachTo(NULL);
}
void CEntity::init() {
  for (auto pEvent: m_lEvents) {
    pEvent->init();
  }
  for (std::list<CEvent*>::iterator it = m_lEvents.begin(); it != m_lEvents.end();) {
    if ((*it)->getEmitter()->getType() == EventEmitter::EMIT_ON_CREATE) {
      (*it)->start();
      delete (*it);
      it = m_lEvents.erase(it);
    }
    else {
      it++;
    }
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

CEntity *CEntity::getRoot() {
  if (!m_pParent) {return this;}
  return m_pParent->getRoot();
}
const CEntity *CEntity::getRoot() const {
  if (!m_pParent) {return this;}
  return m_pParent->getRoot();
}
CEntity *CEntity::getChild(const std::string &sID) {
  for (auto *pChild : m_lChildren) {
    if (pChild->m_sID == sID) {
      return pChild;
    }
  }
  return NULL;
}
CEntity *CEntity::getChildRecursive(const std::string &sID) {
  for (auto *pChild : m_lChildren) {
    if (pChild->m_sID == sID) {
      return pChild;
    }
  }
  for (auto *pChild : m_lChildren) {
    CEntity *pRecursiveChild = pChild->getChildRecursive(sID);
    if (pRecursiveChild) {
      return pRecursiveChild;
    }
  }
  return NULL;
}
const CEntity *CEntity::getChild(const std::string &sID) const {
  for (auto *pChild : m_lChildren) {
    if (pChild->m_sID == sID) {
      return pChild;
    }
  }
  return NULL;
}
const CEntity *CEntity::getChildRecursive(const std::string &sID) const {
  for (auto *pChild : m_lChildren) {
    if (pChild->m_sID == sID) {
      return pChild;
    }
  }
  for (auto *pChild : m_lChildren) {
    const CEntity *pRecursiveChild = pChild->getChildRecursive(sID);
    if (pRecursiveChild) {
      return pRecursiveChild;
    }
  }
  return NULL;
}

void CEntity::destroy() {
  for (std::list<CEvent*>::iterator it = m_lEvents.begin(); it != m_lEvents.end();) {
    if ((*it)->getEmitter()->getType() == EventEmitter::EMIT_ON_DESTROY) {
      (*it)->start();
      delete (*it);
      it = m_lEvents.erase(it);
    }
    else {
      it++;
    }
  }
  m_Map.destroy(this, true);
}

void CEntity::sendCallToAll(void (CEntity::*pFunction)()) {
  (this->*pFunction)();
  for (auto pChild : m_lChildren) {
    pChild->sendCallToAll(pFunction);
  }
}

void CEntity::destroyChildren() {
  while (m_lChildren.size() > 0) {
    CEntity *toDel = m_lChildren.front();
    m_lChildren.pop_front();
    delete toDel;
  }
}

void CEntity::destroyEvent(CEvent *pEvent) {
  assert(pEvent);
  m_lEvents.remove(pEvent);
  delete pEvent;
}

void CEntity::sendMessageToAll(const CMessage &message) {
  handleMessage(message);
  for (auto pChild : m_lChildren) {
    pChild->sendMessageToAll(message);
  }
}

void CEntity::update(Ogre::Real tpf) {
  for (auto &pEnt : m_lChildren) {
    pEnt->update(tpf);
  }

  for (auto it = m_lEvents.begin(); it !=m_lEvents.end();) {
    CEvent *pEvent = *it;
    if (pEvent->getEmitter()->getType() == EventEmitter::EMIT_ON_COLLISION) {
      if (getWorldBoundingBox()
	  .collidesWith(dynamic_cast<const EventEmitter::COnCollision*>(pEvent->getEmitter())
			->getEntity()
			->getWorldBoundingBox())
	  != CCD_NONE) {
	pEvent->start();
	if (pEvent->getEmitter()->getRepeatType() == EventEmitter::REPEAT_NONE) {
	  delete pEvent;
	  it = m_lEvents.erase(it);
	  continue;
	}
      }
      else {
	pEvent->stop();
      }
    }

    it++;
  }
}
void CEntity::render(Ogre::Real tpf) {
  for (auto &pEnt : m_lChildren) {
    pEnt->render(tpf);
  }
}
void CEntity::readFromXMLElement(const tinyxml2::XMLElement *pElem) {
  // clear first
  while (m_lEvents.size() > 0) {
    delete m_lEvents.front();
    m_lEvents.pop_front();
  }


  // now read
  m_sID = Attribute(pElem, "id", m_sID);
  m_uiType = IntAttribute(pElem, "type", 0);
  m_bVisible = BoolAttribute(pElem, "visible", m_bVisible);
  m_vPosition = Vector2Attribute(pElem, "", m_vPosition);
  m_vSize = Vector2Attribute(pElem, "size", m_vSize);
  m_vScale = Vector2Attribute(pElem, "scale", m_vScale);
  m_bbRelativeBoundingBox = BoundingBox2dAttribute(pElem,
						   m_bbRelativeBoundingBox,
						   "relative_");
    
  // read events
  using namespace tinyxml2;
  const XMLElement *pEventsElement = pElem->FirstChildElement("events");
  if (pEventsElement) {
    for (const XMLElement *pEventElement = pEventsElement->FirstChildElement();
	 pEventElement;
	 pEventElement = pEventElement->NextSiblingElement()) {
      m_lEvents.push_back(CEventCreator::create(m_Map, *this, pEventElement));
    }
  }
}
void CEntity::writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const {
  using namespace tinyxml2;

  SetAttribute(pElement, "", m_vPosition); // pos without label
  SetAttribute(pElement, "id", m_sID);
  SetAttribute(pElement, "type", m_uiType);
  if (eStyle == OS_FULL) {
    SetAttribute(pElement, "visible", m_bVisible);
    SetAttribute(pElement, "size", m_vSize);
    SetAttribute(pElement, "scale", m_vScale);
    SetAttribute(pElement, m_bbRelativeBoundingBox, "relative_");
  }

  // write events
  if (m_lEvents.size() > 0) {
    XMLDocument *pDoc = pElement->GetDocument();
    XMLElement *pEventsElement = pDoc->NewElement("events");
    pElement->InsertEndChild(pEventsElement);
    
    for (auto pEvent : m_lEvents) {
      XMLElement *pEventElement = pDoc->NewElement("event");
      pEventsElement->InsertEndChild(pEventElement);
      pEvent->writeToXMLElement(pEventElement, eStyle);
    }
  }
}
void CEntity::handleMessage(const CMessage &message) {
  switch (message.getType()) {
  case CMessage::MT_MESSAGE_BOX_PAGE_CHANGED:
    for (auto pEvent : m_lEvents) {
      if (pEvent->getEmitter()->getType() == EventEmitter::EMIT_ON_MESSAGE_BOX_PAGE_CHANGE) {
	const EventEmitter::COnMessageBoxPageChange *pEmitter(dynamic_cast<const EventEmitter::COnMessageBoxPageChange*>(pEvent->getEmitter()));
	if (pEmitter->getSrcID() == message.getID()
	    && pEmitter->getPageID() == message.getInt()) {
	  pEvent->start();
	}
      }
    }
    break;
  default:
    break;
  }

  for (auto pEvent : m_lEvents) {
    if (pEvent->getEmitter()->getType() == EventEmitter::EMIT_ON_MESSAGE) {
      const EventEmitter::COnMessage *pEmitter(dynamic_cast<const EventEmitter::COnMessage*>(pEvent->getEmitter()));
      if (pEmitter->getMessageType() == message.getType()) {
	pEvent->start();
      }
    }
  }
}










