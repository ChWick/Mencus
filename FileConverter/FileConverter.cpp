#include "../Game/Grid2d.hpp"
#include <iostream>
#include <fstream>
#include <tinyxml2.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <sstream>

using namespace boost;
using namespace tinyxml2;
using namespace std;

int main() {
  grid2d<int> tiles;

  string buffer;
  ifstream inputfile;
  ofstream outputfile;

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
  getline(inputfile, buffer); // [Background]
  getline(inputfile, buffer); //
  getline(inputfile, buffer); // background type
  if (buffer == "Wald") {
    pMapElem->SetAttribute("background", "forest");
  }
  else {
    cout << buffer << " was not yet set as background type" << endl;
    return -1;
  }
  getline(inputfile, buffer); //

  // Size
  // ============================================================
  getline(inputfile, buffer); // [Size]
  getline(inputfile, buffer); //
  getline(inputfile, buffer); // x
  int sizex = atoi(buffer.c_str());
  getline(inputfile, buffer); // y
  int sizey = atoi(buffer.c_str());
  cout << "Map size = " << sizex << ", " << sizey << endl;
  getline(inputfile, buffer); //
  tiles.resize(sizex, sizey);
  pMapElem->SetAttribute("sizex", sizex);
  pMapElem->SetAttribute("sizey", sizey);

  // Tiles
  // ============================================================
  XMLElement *pTilesElem = doc.NewElement("tiles");
  pMapElem->InsertEndChild(pTilesElem);

  getline(inputfile, buffer); // [Tiles]
  getline(inputfile, buffer); //
  for (int y = 0; y < sizey; y++) {
    vector<string> splitted;
    getline(inputfile, buffer);
    split(splitted, buffer, is_any_of(" "), token_compress_on);
    if (splitted.size() != sizex + 1) {
      cout << "Wrong line count at y = " << y << endl;
      return -1;
    }
    int usedy = atoi(splitted[0].c_str());
    for (int x = 0; x < sizex; x++) {
      tiles(x, usedy) = atoi(splitted[x + 1].c_str());
    }
  }
  getline(inputfile, buffer); //

  // invert order and print to xml
  pTilesElem->SetAttribute("invert", "false");
  for (int y = sizey - 1; y >= 0; y++) {
    stringstream ss;
    for (int x = 0; x < sizex - 1; x++) {
      ss << tiles(x, y) << " ";
    }
    ss << tiles(sizex - 1, y);
    XMLElement *pRowElem = doc.NewElement("row");
    pTilesElem->InsertEndChild(pRowElem);
    pRowElem->SetAttribute("tiles", ss.str().c_str());
  }

  // objects
  // ============================================================


  return 1;
}
