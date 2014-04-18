#ifndef _STATISTICS_HPP_
#define _STATISTICS_HPP_

#include "Weapon.hpp"
#include <string>
#include <OgreException.h>
#include <tinyxml2.h>
#include <XMLHelper.hpp>

enum EMissionState {
  MS_ACCOMPLISHED,		//!< This mission has been accomplished
  MS_FAILED,			//!< This mission has been played, but failed
  MS_SKIPPED			//!< This mission has been skipped
};
struct SStatistics {
  static EMissionState parseMissionState(const std::string &s) {
    if (s == "skipped") {
      return MS_SKIPPED;
    }
    else if (s == "accomplished") {
      return MS_ACCOMPLISHED;
    }
    return MS_FAILED;
  }
  static std::string toString(const EMissionState ms) {
    switch (ms) {
    case MS_ACCOMPLISHED:
      return "accomplished";
    case MS_FAILED:
      return "failed";
    case MS_SKIPPED:
      return "skipped";
    }

    throw Ogre::Exception(0, "Mission state could not be converted to string", __FILE__);
  }

  SStatistics(const std::string &name = "unknown level file name") 
    : sLevelFileName(name), 
      eMissionState(MS_FAILED),
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
