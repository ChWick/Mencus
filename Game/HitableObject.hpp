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

#ifndef HITABLE_OBJECT_HPP
#define HITABLE_OBJECT_HPP

#include <algorithm>
#include <tinyxml2.h>
#include "XMLHelper.hpp"
#include "OutputStyle.hpp"

class CHitableObject {
private:
  Ogre::Real m_fMaximumHitpoints;
  Ogre::Real m_fHitpoints;
  bool m_bInvunerable;
public:
  CHitableObject(Ogre::Real fMaxHitpoints)
    : m_fMaximumHitpoints(fMaxHitpoints),
      m_fHitpoints(fMaxHitpoints),
      m_bInvunerable(false) {
  }
  CHitableObject(const tinyxml2::XMLElement *pElem)
    : m_fMaximumHitpoints(XMLHelper::RealAttribute(pElem, "hp", 0, true)),
      m_fHitpoints(XMLHelper::RealAttribute(pElem, "curhp", m_fMaximumHitpoints)),
      m_bInvunerable(XMLHelper::BoolAttribute(pElem, "invunerable", false)) {
  }

  void init() {
    if (m_fHitpoints < m_fMaximumHitpoints) {
      damageTakenCallback(0);
    }
    if (m_fHitpoints <= 0) {
      killedByDamageCallback();
    }
  }

  void takeDamage(Ogre::Real fHitpoints) {
    if (m_bInvunerable) {return;}

    m_fHitpoints -= fHitpoints;
    damageTakenCallback(fHitpoints);
    if (m_fHitpoints <= 0) {
      killedByDamageCallback();
    }
  }

  void setHitpoints(Ogre::Real fHitpoints) {m_fHitpoints = std::min<Ogre::Real>(fHitpoints, m_fMaximumHitpoints);}

  Ogre::Real getHitpoints() const {return m_fHitpoints;}
  Ogre::Real getMaximumHitpoints() const {return m_fMaximumHitpoints;}
  Ogre::Real &getMaximumHitpoints() {return m_fMaximumHitpoints;}
  
  bool isInvunerable() const {return m_bInvunerable;}
  void setInvunerable(bool bInvunerable) {m_bInvunerable = bInvunerable;}
  void addHitpoints(Ogre::Real fHitpoints) {m_fHitpoints = std::min<Ogre::Real>(m_fHitpoints + fHitpoints, m_fMaximumHitpoints);}


  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
    pElem->SetAttribute("hp", getMaximumHitpoints());
    if (eStyle == OS_FULL) {
      pElem->SetAttribute("curhp", getHitpoints());
      pElem->SetAttribute("invunerable", isInvunerable());
    }
  }
protected:
  virtual void damageTakenCallback(Ogre::Real fHitpoints) {}
  virtual void killedByDamageCallback() {}
};

#endif
