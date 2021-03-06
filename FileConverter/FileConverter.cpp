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

#include "../Game/Grid2d.hpp"
#include <iostream>
#include <fstream>
#include <tinyxml2.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <stdio.h>

using namespace boost;
using namespace tinyxml2;
using namespace std;

int sizex;
int sizey;

const float OBJECT_HEIGHTS[] = {
  0.5,
  0.5,
  0.5,
  0.25,
  1,
  1
};
const float SWITCH_HEIGHTS[] = {
  0.25,
  1
};
const float PLAYER_HEIGHT = 2;
const float ENEMY_HEIGHTS[] = {
  1,
  2,
  2
};

float toNewX(int x) {
  return static_cast<float>(x) / 32.f;
}
float toNewY(int y) {
  return sizey - static_cast<float>(y) / 32.f;
}
int toNewTileY(int y) {
  return sizey - 1 - y;
}
int toDirection(int i) {
  return (i == 1) ? 1 : -1;
}
string toBool(const string &s, bool invert = false) {
  if (invert) {
    if (s == "0") {
      return "true";
    }
    return "false";
  }
  if (s == "1") {
    return "true";
  }
  return "false";
}
int convertEnemyId(int id) {
  return id;
}
int convertObjectId(int id) {
  if (id == 0) {return 3;} // Key
  if (id == 3) {return 0;} // bomb
  if (id == 5) {return 1;} // health potion
  if (id == 6) {return 2;} // mana potion
  if (id == 1) {return 5;} // torch 1
  if (id == 2) {return 5;} // torch 2 idendical
  if (id == 4) {return 4;} // scratch

  cout << "Object id " << id << "is unknown!" << endl;
  return id;
}

void readLine(ifstream &stream, string &out) {
  getline(stream, out);
  trim(out);
  if (out[out.length() - 1] == '\r') {
    out = out.substr(0, out.length() - 1);
  }
}
int main(int argc, const char* argv[]) {
  // print licence
  cout <<
    "FileConverter  Copyright (C) 2014 Christoph Wick" << endl <<
    "This program comes with ABSOLUTELY NO WARRANTY." << endl <<
    "This is free software, and you are welcome to redistribute it" << endl <<
    "under certain conditions. See license!" << endl;

  // Prints each argument on the command line.
	for( int i = 0; i < argc; i++ )
	{
		printf( "arg %d: %s\n", i, argv[i] );
	}

  if (argc != 3) {
    cout << "specify input and output path" << endl;
    return 1;
  }
  grid2d<int> tiles;

  string buffer;
  ifstream inputfile(argv[1]);
  ofstream outputfile(argv[2]);

  if (!inputfile) {
    cout << "Input file could not be opened!" << endl;
    return -1;
  }
  if (!outputfile) {
    cout << "Output file could not be opened!" << endl;
    return -1;
  }


  XMLDocument doc;
  XMLElement *pMapElem = doc.NewElement("map");
  doc.InsertEndChild(pMapElem);

  // Background
  // ============================================================
  readLine(inputfile, buffer); // [Background]
  cout << buffer << " = ";
  readLine(inputfile, buffer); //
  readLine(inputfile, buffer); // background type
  cout << buffer << endl;
  if (buffer == "Wald") {
    pMapElem->SetAttribute("background", "forest");
  }
  else if (buffer == "Nebel") {
    pMapElem->SetAttribute("background", "fog");
  }
  else if (buffer == "Sternenhimmel") {
    pMapElem->SetAttribute("background", "stars");
  }
  else if (buffer == "Wolken") {
    pMapElem->SetAttribute("background", "clouds");
  }
  else {
    cout << buffer << " was not yet set as background type" << endl;
    return -1;
  }
  readLine(inputfile, buffer); //

  // Size
  // ============================================================
  readLine(inputfile, buffer); // [Size]
  cout << buffer << " = ";
  readLine(inputfile, buffer); //
  readLine(inputfile, buffer); // x
  sizex = atoi(buffer.c_str());
  readLine(inputfile, buffer); // y
  sizey = atoi(buffer.c_str());
  cout << sizex << ", " << sizey << endl;
  readLine(inputfile, buffer); //
  tiles.resize(sizex, sizey);
  pMapElem->SetAttribute("sizex", sizex);
  pMapElem->SetAttribute("sizey", sizey);

  // Tiles
  // ============================================================
  XMLElement *pTilesElem = doc.NewElement("tiles");
  pMapElem->InsertEndChild(pTilesElem);

  readLine(inputfile, buffer); // [Tiles]
  cout << buffer << endl;
  readLine(inputfile, buffer); //
  for (int y = 0; y < sizey; y++) {
    cout << "Readling row " << y << endl;
    vector<string> splitted;
    readLine(inputfile, buffer);
    split(splitted, buffer, is_any_of(" \r\n\t"), token_compress_on);
    if (splitted.size() != static_cast<size_t>(sizex + 1)) {
      cout << "Wrong line count at y = " << y << ". Given size was " << splitted.size() << endl;
      return -1;
    }
    int usedy = atoi(splitted[0].c_str());
    for (int x = 0; x < sizex; x++) {
      tiles(x, usedy) = atoi(splitted[x + 1].c_str());
    }
  }
  readLine(inputfile, buffer); //

  // invert order and print to xml
  cout << "Writing to xml file..." << endl;
  pTilesElem->SetAttribute("invert", "true");
  for (int y = sizey - 1; y >= 0; y--) {
    stringstream ss;
    for (int x = 0; x < sizex - 1; x++) {
      ss << tiles(x, y) << " ";
    }
    ss << tiles(sizex - 1, y);
    XMLElement *pRowElem = doc.NewElement("row");
    pTilesElem->InsertFirstChild(pRowElem);
    pRowElem->SetAttribute("tiles", ss.str().c_str());
  }
  cout << "Done" << endl;

  // enemies and player
  // ============================================================
  readLine(inputfile, buffer); // [Enemies]
  cout << buffer << " = ";
  readLine(inputfile, buffer); //
  readLine(inputfile, buffer); // count
  int enemycount = atoi(buffer.c_str());
  cout << enemycount << endl;
  readLine(inputfile, buffer); //

  XMLElement *pEnemiesElem = doc.NewElement("enemies");
  pMapElem->InsertEndChild(pEnemiesElem);

  XMLElement *pPlayerElem = doc.NewElement("player");
  pMapElem->InsertFirstChild(pPlayerElem);

  // player
  vector<string> splitted;
  readLine(inputfile, buffer);
  split(splitted, buffer, is_any_of(" \r\n\t"), token_compress_on);
  pPlayerElem->SetAttribute("posx", toNewX(atoi(splitted[1].c_str())));
  pPlayerElem->SetAttribute("posy", toNewY(atoi(splitted[2].c_str())) - PLAYER_HEIGHT);
  pPlayerElem->SetAttribute("direction", toDirection(atoi(splitted[3].c_str())));

  for (int i = 0; i < enemycount; i++) {
    XMLElement *pEnemyElem = doc.NewElement("enemy");
    pEnemiesElem->InsertEndChild(pEnemyElem);

    readLine(inputfile, buffer);
    split(splitted, buffer, is_any_of(" \r\n\t"), token_compress_on);

    int id = convertEnemyId(atoi(splitted[0].c_str()));
    pEnemyElem->SetAttribute("type", id);
    pEnemyElem->SetAttribute("x", toNewX(atoi(splitted[1].c_str())));
    pEnemyElem->SetAttribute("y", toNewY(atoi(splitted[2].c_str())) - ENEMY_HEIGHTS[id]);
    pEnemyElem->SetAttribute("direction", toDirection(atoi(splitted[3].c_str())));
    pEnemyElem->SetAttribute("jumps", atoi(splitted[4].c_str()));
    pEnemyElem->SetAttribute("hp", atof(splitted[7].c_str()));
    pEnemyElem->SetAttribute("rank", atoi(splitted[8].c_str()));
  }
  readLine(inputfile, buffer); //
  // switches
  // ============================================================
  readLine(inputfile, buffer); // [Switches]
  cout << buffer << " = ";
  readLine(inputfile, buffer); //
  readLine(inputfile, buffer); // count
  int switchescount = atoi(buffer.c_str());
  cout << switchescount << endl;
  readLine(inputfile, buffer); //

  XMLElement *pSwitchesElem = doc.NewElement("switches");
  pMapElem->InsertEndChild(pSwitchesElem);

  for (int i = 0; i < switchescount; i++) {
    XMLElement *pSwitchElem = doc.NewElement("switch");
    pSwitchesElem->InsertEndChild(pSwitchElem);

    readLine(inputfile, buffer);
    split(splitted, buffer, is_any_of(" \r\n\t"), token_compress_on);

    int id = atoi(splitted[0].c_str()) - 1;
    pSwitchElem->SetAttribute("type", atoi(splitted[0].c_str()) - 1);
    pSwitchElem->SetAttribute("x", toNewX(atoi(splitted[1].c_str())));
    pSwitchElem->SetAttribute("y", toNewY(atoi(splitted[2].c_str())) - SWITCH_HEIGHTS[id]);

    int tileChanges = atoi(splitted[3].c_str());
    for (int t = 0; t < tileChanges; t++) {
      XMLElement *pTileChangeElem = doc.NewElement("changes");
      pSwitchElem->InsertEndChild(pTileChangeElem);
      pTileChangeElem->SetAttribute("x", atoi(splitted[4 + t * 3].c_str()));
      pTileChangeElem->SetAttribute("y", toNewTileY(atoi(splitted[5 + t * 3].c_str())));
      pTileChangeElem->SetAttribute("id", atoi(splitted[6 + t * 3].c_str()));
    }

    pSwitchElem->SetAttribute("affectsBlocks", toBool(splitted[4 + 3 * tileChanges], true).c_str());
  }

  if (switchescount != 0) {
    readLine(inputfile, buffer); //
  }
  // links
  // ============================================================
  readLine(inputfile, buffer); // [Links]
  cout << buffer << " = ";
  readLine(inputfile, buffer); //
  readLine(inputfile, buffer); // count
  int linkscount = atoi(buffer.c_str());
  cout << linkscount << endl;
  readLine(inputfile, buffer); //

  XMLElement *pLinksElem = doc.NewElement("links");
  pMapElem->InsertEndChild(pLinksElem);

  for (int i = 0; i < linkscount; i++) {
    XMLElement *pLinkElem = doc.NewElement("link");
    pLinksElem->InsertEndChild(pLinkElem);

    readLine(inputfile, buffer);
    split(splitted, buffer, is_any_of(" \r\n\t"), token_compress_on);

    pLinkElem->SetAttribute("fromx", atoi(splitted[0].c_str()));
    pLinkElem->SetAttribute("fromy", toNewTileY(atoi(splitted[1].c_str())));
    pLinkElem->SetAttribute("tox", atoi(splitted[2].c_str()));
    pLinkElem->SetAttribute("toy", toNewTileY(atoi(splitted[3].c_str())));
  }

  if (linkscount != 0) {
    readLine(inputfile, buffer); //
  }
  // objects
  // ============================================================
  readLine(inputfile, buffer); // [Other Objects]
  cout << buffer << " = ";
  readLine(inputfile, buffer); //
  readLine(inputfile, buffer); // count
  int objectscount = atoi(buffer.c_str());
  cout << objectscount << endl;
  readLine(inputfile, buffer); //

  XMLElement *pObjectsElem = doc.NewElement("objects");
  pMapElem->InsertEndChild(pObjectsElem);

  for (int i = 0; i < objectscount; i++) {
    XMLElement *pObjectElem = doc.NewElement("object");
    pObjectsElem->InsertEndChild(pObjectElem);

    readLine(inputfile, buffer);
    split(splitted, buffer, is_any_of(" \r\n\t"), token_compress_on);

    int objectid = convertObjectId(atoi(splitted[0].c_str()));
    pObjectElem->SetAttribute("type", objectid);
    pObjectElem->SetAttribute("x", toNewX(atoi(splitted[1].c_str())));
    pObjectElem->SetAttribute("y", toNewY(atoi(splitted[2].c_str())) - OBJECT_HEIGHTS[objectid]);
  }

  if (objectscount != 0) {
    readLine(inputfile, buffer); //
  }

  // endangered tiles
  // ============================================================
  readLine(inputfile, buffer); // [endangered tiles]
  cout << buffer << " = ";
  readLine(inputfile, buffer); //
  readLine(inputfile, buffer); // count
  int endangeredTilesCount = atoi(buffer.c_str());
  cout << endangeredTilesCount << endl;
  readLine(inputfile, buffer); //

  XMLElement *pETsElem = doc.NewElement("endangeredTiles");
  pMapElem->InsertEndChild(pETsElem);

  for (int i = 0; i < endangeredTilesCount; i++) {
    XMLElement *pETElem = doc.NewElement("endangeredTile");
    pETsElem->InsertEndChild(pETElem);

    readLine(inputfile, buffer);
    split(splitted, buffer, is_any_of(" \r\n\t"), token_compress_on);

    pETElem->SetAttribute("x", atoi(splitted[0].c_str()));
    pETElem->SetAttribute("y", toNewTileY(atoi(splitted[1].c_str())));
    pETElem->SetAttribute("targetTile", atoi(splitted[2].c_str()));
  }

  if (endangeredTilesCount != 0) {
    readLine(inputfile, buffer); //
  }

  // output
  // ============================================================
  XMLPrinter xmlprinter;
  doc.Accept(&xmlprinter);

  outputfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  outputfile << xmlprinter.CStr();

  return 1;
}
