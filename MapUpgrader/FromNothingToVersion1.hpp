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
  EUpgradeResult upgradeChangeTileType(XMLElement *pElem) {
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
