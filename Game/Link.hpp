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

#ifndef LINK_HPP
#define LINK_HPP

#include <Ogre.h>
#include <string>
#include "Entity.hpp"
#include "IDGenerator.hpp"
#include "XMLHelper.hpp"

class CLink : public CEntity {
public:
  enum ELinkDirection {
    LD_FIRST_TO_SECOND    = 1,
    LD_SECOND_TO_FIRST    = 2,
    LD_BOTH               = 3
  };

  static std::string toString(ELinkDirection eLD) {
    switch (eLD) {
    case LD_FIRST_TO_SECOND:
      return "first_to_second";
    case LD_SECOND_TO_FIRST:
      return "second_to_first";
    default:
      break;
    }
    return "both";
  }
  static ELinkDirection parseLinkDirection(const std::string &str) {
    if (str == "first_to_second") {return LD_FIRST_TO_SECOND;}
    if (str == "second_to_first") {return LD_SECOND_TO_FIRST;}
    if (str == "both") {return LD_BOTH;}
    throw Ogre::Exception(0, "Invalid direction in link: " + str, __FILE__);
  }
private:
  unsigned int m_uiFirstX;
  unsigned int m_uiFirstY;
  unsigned int m_uiSecondX;
  unsigned int m_uiSecondY;
  bool m_bActivated;
  ELinkDirection m_eLinkDirection;
public:
  CLink(CMap &map,
	CEntity *pParent,
	const tinyxml2::XMLElement *pElem)
    : CEntity(map, pParent, pElem),
      m_uiFirstX(XMLHelper::IntAttribute(pElem, "fromx")),
      m_uiFirstY(XMLHelper::IntAttribute(pElem, "fromy")),
      m_uiSecondX(XMLHelper::IntAttribute(pElem, "tox")),
      m_uiSecondY(XMLHelper::IntAttribute(pElem, "toy")),
      m_bActivated(XMLHelper::BoolAttribute(pElem, "activated", true)),
      m_eLinkDirection(parseLinkDirection(XMLHelper::Attribute(pElem, "direction", "both"))) {
  }
  CLink(CMap &map,
	CEntity *pParent)
    : CEntity(map, CIDGenerator::nextID("Link_"), pParent),
      m_uiFirstX(0), m_uiFirstY(0),
      m_uiSecondX(1), m_uiSecondY(1),
      m_bActivated(true),
      m_eLinkDirection(LD_BOTH) {
  }

  bool operator==(const CLink &link) const {
    return m_uiFirstX == link.m_uiFirstX && m_uiFirstY == link.m_uiFirstY &&
      m_uiSecondX == link.m_uiSecondX && m_uiSecondY == link.m_uiSecondY &&
      m_sID == link.m_sID && m_bActivated == link.m_bActivated &&
      m_eLinkDirection == link.m_eLinkDirection;
  }

  unsigned int getFirstX() const {return m_uiFirstX;}
  unsigned int getFirstY() const {return m_uiFirstY;}

  unsigned int getSecondX() const {return m_uiSecondX;}
  unsigned int getSecondY() const {return m_uiSecondY;}


  unsigned int &getFirstX() {return m_uiFirstX;}
  unsigned int &getFirstY() {return m_uiFirstY;}

  unsigned int &getSecondX() {return m_uiSecondX;}
  unsigned int &getSecondY() {return m_uiSecondY;}

  bool isActivated() const {return m_bActivated;}
  void setActivated(bool b) {m_bActivated = b;}
  bool &getActivated() {return m_bActivated;}

  ELinkDirection getLinkDirection() const {return m_eLinkDirection;}

  std::string toString() const {
    return "Link with id (" + m_sID + ") from (" + Ogre::StringConverter::toString(m_uiFirstX) +
      ", " + Ogre::StringConverter::toString(m_uiFirstY) + ") to (" +
      Ogre::StringConverter::toString(m_uiSecondX) + ", " +
      Ogre::StringConverter::toString(m_uiSecondY) + ")";
  }

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
    CEntity::writeToXMLElement(pElem, eStyle);
    
    using namespace XMLHelper;
    
    SetAttribute(pElem, "fromx", m_uiFirstX);
    SetAttribute(pElem, "fromy", m_uiFirstY);
    SetAttribute(pElem, "tox", m_uiSecondX);
    SetAttribute(pElem, "toy", m_uiSecondY);
    SetAttribute(pElem, "activated", m_bActivated);
    SetAttribute(pElem, "direction", CLink::toString(m_eLinkDirection));
  }
};

#endif
