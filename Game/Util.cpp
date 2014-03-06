#include "Util.hpp"
#include <Ogre.h>


std::string getShotTexture(const std::string &sType, const std::string &sName, int index) {
  if (index < 0) {
    return "shots/" + sType + "/" + sName + ".png";
  }
  return "shots/" + sType + "/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
}
std::string getSkullTexture(std::string sName, int index) {return getShotTexture("skull", sName, index);}
std::string getBoltTexture(std::string sName, int index) {return getShotTexture("bolt", sName, index);}
std::string getColumnTexture(std::string sName, int index) {return getShotTexture("column", sName, index);}
std::string getHUDTexturePath(std::string sName) {
  return "hud/" + sName + ".png";
}
std::string getBombTexture(std::string sName, int index) {
  if (index < 0) {
    return "objects/bomb/" + sName + ".png";
  }
  return "objects/bomb/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
}
std::string getSwitchTexture(unsigned int uiSwitchID, bool on) {
  return "objects/switch/switch"
    + Ogre::StringConverter::toString(uiSwitchID + 1)
    + "/" + ((on) ? "On" : "Off") + ".png";
}
std::string getTextureInGfx(std::string sTextureName) {
  return "" + sTextureName;
}
std::string getPlayerTexturePath(std::string sName, int index) {
  if (index < 0) {
    return "mencus/" + sName + ".png";
  }
  else {
    return "mencus/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
  }
}
std::string getTileTexturePath(unsigned int uiTileID) {
  return "tiles/Tile" + Ogre::StringConverter::toString(uiTileID) + ".png";
}
std::string getBackgroundTexturePath(std::string sName) {
  return "background/" + sName + ".jpg";
}
std::string getOtherObjectsTexturePath(std::string sName) {
  return "objects/other/" + sName + ".png";
}
std::string getTorchTexturePath(std::string sName, int index) {
  if (index < 0) {
    return "objects/torch/" + sName + ".png";
  }
  return "objects/torch/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
}
