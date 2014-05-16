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

#ifndef CAMERA_RESTRICTION
#define CAMERA_RESTRICTION

#include <OgreVector2.h>
#include <algorithm>
#include "XMLHelper.hpp"

class CCameraRestriction {
public:
  enum ETypes {
    HORIZONTAL_RESTRICTION,
    VERTICAL_RESTRICTION
  };
  static Ogre::String toString(ETypes type) {
    switch (type) {
    case HORIZONTAL_RESTRICTION:
      return "horizontal";
    case VERTICAL_RESTRICTION:
      return "vertical";
    default:
      throw Ogre::Exception(0, "CameraRestriction-type undefined", __FILE__);
    }
  }
private:
  const ETypes m_eRestrictionType;
  const float m_fLevel;
  const Ogre::Vector2 &m_vScreensize;

public:
  CCameraRestriction(const ETypes eRestrictionType, const Ogre::Vector2 &vScreensize, float fLevel)
    : m_eRestrictionType(eRestrictionType),
      m_fLevel(fLevel),
      m_vScreensize(vScreensize) {
  }

  CCameraRestriction(const CCameraRestriction &cr)
    : m_eRestrictionType(cr.m_eRestrictionType),
      m_fLevel(cr.m_fLevel),
      m_vScreensize(cr.m_vScreensize) {
  }

  void update(Ogre::Vector2 &vCameraPos, const Ogre::Vector2 &vPlayerCenter) {
    switch (m_eRestrictionType) {
    case HORIZONTAL_RESTRICTION:
      if (vPlayerCenter.y > m_fLevel) {
        vCameraPos.y = std::max<Ogre::Real>(vCameraPos.y, m_fLevel);
      }
      else {
        vCameraPos.y = std::min<Ogre::Real>(vCameraPos.y, m_fLevel);
      }
      break;
    case VERTICAL_RESTRICTION:
      if (vPlayerCenter.x > m_fLevel) {
        vCameraPos.x = std::max<Ogre::Real>(vCameraPos.x, m_fLevel);
      }
      else {
        vCameraPos.x = std::min<Ogre::Real>(vCameraPos.x, m_fLevel);
      }
      break;
    }
  }
  CCameraRestriction& operator=(const CCameraRestriction&) {
    return *this;
  }

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem) const {
    using namespace XMLHelper;

    pElem->SetAttribute("type", toString(m_eRestrictionType).c_str());
    switch (m_eRestrictionType) {
    case HORIZONTAL_RESTRICTION:
      SetAttribute(pElem, "y", m_fLevel);
      break;
    case VERTICAL_RESTRICTION:
      SetAttribute(pElem, "x", m_fLevel);
      break;
    default:
      throw Ogre::Exception(0, "Undefined CameraRestriction-type", __FILE__);
    }
  }
};

#endif // CAMERA_RESTRICTION
