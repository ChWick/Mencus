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

#include <iostream>
#include <fstream>
#include <string>
#include <tinyxml2.h>
#include "FromNothingToVersion1.hpp"
#include "FromVersion1To2.hpp"
#include "UpgradeResult.hpp"

using namespace std;
using namespace tinyxml2;

//! upgrades a map file to the next higher version
/**
  * \returns false if no more upgrades are possible or an error occured
  */
EUpgradeResult upgradeStep(XMLDocument &doc) {
  XMLElement *pElem = doc.FirstChildElement();
  if (!pElem) {
    cout << "Missing root element" << endl;
    return UPGRADE_ERROR;
  }

  string s;
  if (pElem->Attribute("version")) {s = pElem->Attribute("version");}

  if (s.empty()) {
    FromNothingToVersion1 v;
    return v.run(doc);
  }
  else if (s == "1") {
    FromVersion1To2 v;
    return v.run(doc);
  }
  else if (s == "2") {
    return UPGRADE_FINISHED; // done
  }

  cout << "unknown version string: '" << s << "'" << endl;
  return UPGRADE_ERROR;
} 

// upgrades a map file to the current version
void upgradeFull(const string &filepath) {
  cout << "Starting full upgrade of " << filepath << endl;
  ifstream file(filepath);
  if (!file) {
    cout << "   coult not be opened" << endl;
    return;
  }
  XMLDocument doc;
  doc.LoadFile(filepath.c_str());
  if (doc.Error()) {
    cout << "   error while parsing the xml document" << endl;
    cout << "   error id: " << doc.ErrorID() << endl;
    //cout << "   " << doc.GetErrorStr1() << endl;
    return;
  }
  EUpgradeResult r;
  do {
    r = upgradeStep(doc);
  }
  while (r == UPGRADE_SUCCESSFULL); //  upgrade permanet

  if (r == UPGRADE_ERROR) {
    cout << "Error while upgrading, cancelled." << endl;
  }
  else if (r == UPGRADE_FINISHED) {
    cout << "Saving new file" << endl;
    doc.SaveFile(filepath.c_str());
    cout << "done." << endl;
  }
}

int main(int argc, char **argv) {
  for (int f = 1; f < argc; f++) {
    upgradeFull(argv[f]);
  }

  return 1;
}
