#ifndef FROM_NOTHING_TO_VERSION1_HPP
#define FROM_NOTHING_TO_VERSION1_HPP

#include <tinyxml2.h>
#include <iostream>
#include "UpgradeResult.hpp"

using namespace tinyxml2;
using namespace std;

class FromNothingToVersion1 {
private:
  int m_iCounter;
public:
  //! upgrades a map without a version (before it was introduced)
  //! to the default version
  EUpgradeResult run(XMLDocument &doc) {
    m_iCounter = 0;
    cout << "Upgrade from nothing to version 1" << endl;

    XMLElement *pElem = doc.RootElement();
    if (upgradeChangeTileType(pElem) == UPGRADE_ERROR) {
      return UPGRADE_ERROR;
    }
    
    // finally upgrade the version id
    pElem->SetAttribute("version", "1");

    cout << "Upgraded " << m_iCounter << " events" << endl; 
    cout << "Upgrade successfull" << endl;
    
    return UPGRADE_SUCCESSFULL;
  }
private:
  bool upgradeChangeTileType(XMLElement *pElem) {
    if (strcmp(pElem->Value(), "event") == 0 &&
	pElem->Attribute("type") &&
	strcmp(pElem->Attribute("type"), "change_tile") == 0) {
      if (!pElem->Attribute("id")) {
	cout << "Missing id idendifier in change tile event" << endl;
	return UPGRADE_ERROR;
      }
      pElem->SetAttribute("tile", pElem->Attribute("id"));
      pElem->SetAttribute("id", "unset");

      m_iCounter++;
    }

    for (XMLElement *pChild = pElem->FirstChildElement();
	 pChild; pChild = pChild->NextSiblingElement()) {
      if (upgradeChangeTileType(pChild) == UPGRADE_ERROR) {
	return UPGRADE_ERROR;
      }
    }
    return UPGRADE_SUCCESSFULL;
  }
};
#endif
