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

#ifndef FROM_VERSION_1_TO_2
#define FROM_VERSION_1_TO_2

#include "UpgradeBase.hpp"

class FromVersion1To2 : public UpgradeBase {
public:
  FromVersion1To2() :
    UpgradeBase(1, 2) {
  }
  //! upgrades a map without a version (before it was introduced)
  //! to the default version
  EUpgradeResult run_impl(XMLDocument &doc) {
    XMLElement *pElem = doc.RootElement();
    
    return upgradeObject(pElem);
  }
private:
  EUpgradeResult upgradeObject(XMLElement *pElem) {
    if (strcmp(pElem->Value(), "object") == 0 &&
	pElem->Attribute("phy_gravity") == nullptr) {
      pElem->SetAttribute("phy_gravity", 0); // not moving normally

      m_iCounter++;
    }

    for (XMLElement *pChild = pElem->FirstChildElement();
	 pChild; pChild = pChild->NextSiblingElement()) {
      if (upgradeObject(pChild) == UPGRADE_ERROR) {
	return UPGRADE_ERROR;
      }
    }
    return UPGRADE_SUCCESSFULL;
  }
};

#endif
