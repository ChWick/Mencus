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

#ifndef _STATISTICS_HPP_
#define _STATISTICS_HPP_

#include "Weapon.hpp"
#include <string>
#include <OgreException.h>
#include <tinyxml2.h>
#include <XMLHelper.hpp>

enum EMissionState {
  MS_NOT_PLAYED,	        //!< This mission has never been played 
  MS_FAILED,			//!< This mission has been failed
  MS_SKIPPED,			//!< This mission has been skipped
  MS_ACCOMPLISHED,		//!< This mission has been accomplished
};
struct SStatistics {
  static EMissionState parseMissionState(const std::string &s) {
    if (s == "skipped") {
      return MS_SKIPPED;
    }
    else if (s == "accomplished") {
      return MS_ACCOMPLISHED;
    }
    else if (s == "failed") {
      return MS_FAILED;
    }
    return MS_NOT_PLAYED;
  }
  static std::string toString(const EMissionState ms) {
    switch (ms) {
    case MS_ACCOMPLISHED:
      return "accomplished";
    case MS_FAILED:
      return "failed";
    case MS_SKIPPED:
      return "skipped";
    case MS_NOT_PLAYED:
      return "not_played";
    }

    throw Ogre::Exception(0, "Mission state could not be converted to string", __FILE__);
  }

  SStatistics(const std::string &name = "unknown level file name") 
    : sLevelFileName(name), 
      eMissionState(MS_NOT_PLAYED),
      fTime(0),
      fHitpoints(0),
      fLostHitpoints(0),
      fUsedManapoints(0) {
    for (auto &ic : uiUsedItems) {
      ic = 0;
    }
  }
  SStatistics(const tinyxml2::XMLElement *pElem) {
    readFromXMLElement(pElem);
  }

  void readFromXMLElement(const tinyxml2::XMLElement *pElem) {
    using namespace XMLHelper;
    sLevelFileName = Attribute(pElem, "level", "unknown level file name");
    eMissionState = parseMissionState(Attribute(pElem, "mission_state", "failed"));
    fTime = RealAttribute(pElem, "time", 0);
    fHitpoints = RealAttribute(pElem, "hitpoints", 0);
    fManapoints = RealAttribute(pElem, "manapoints", 0);
    fLostHitpoints = RealAttribute(pElem, "lost_hitpoints", 0);
    fUsedManapoints = RealAttribute(pElem, "used_manapoints", 0);

    for (unsigned int i = 0; i < Weapon::I_COUNT; i++) {
      uiUsedItems[i] = IntAttribute(pElem, ("used_" + Weapon::toString(i)).c_str(), 0);
    }
  }

  void writeToXML(tinyxml2::XMLElement *pElem) const {
    assert(pElem);

    pElem->SetAttribute("level", sLevelFileName.c_str());
    pElem->SetAttribute("mission_state", toString(eMissionState).c_str());
    pElem->SetAttribute("time", fTime);
    pElem->SetAttribute("hitpoints", fHitpoints);
    pElem->SetAttribute("manapoints", fManapoints);
    pElem->SetAttribute("lost_hitpoints", fLostHitpoints);
    pElem->SetAttribute("used_manapoints", fUsedManapoints);
    for (unsigned int i = 0; i < Weapon::I_COUNT; i++) {
      pElem->SetAttribute(("used_" + Weapon::toString(i)).c_str(), uiUsedItems[i]);
    }
  }
  
  std::string sLevelFileName;
  EMissionState eMissionState;

  float fTime;
  
  // player data
  float fHitpoints;
  float fManapoints;
  float fLostHitpoints;
  float fUsedManapoints;

  unsigned int uiUsedItems[Weapon::I_COUNT];
};

#endif
