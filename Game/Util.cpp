#include "Util.hpp"
#include <Ogre.h>

std::string getBombTexture(std::string sName, int index) {
  if (index < 0) {
    return "../gfx/objects/bomb/" + sName + ".png";
  }
  return "../gfx/objects/bomb/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
}
std::string getSwitchTexture(unsigned int uiSwitchID, bool on) {
  return "../gfx/objects/switch/switch"
    + Ogre::StringConverter::toString(uiSwitchID + 1)
    + "/" + ((on) ? "On" : "Off") + ".png";
}
std::string getTextureInGfx(std::string sTextureName) {
  return "../gfx/" + sTextureName;
}
std::string getPlayerTexturePath(std::string sName, int index) {
  if (index < 0) {
    return "../gfx/mencus/" + sName + ".png";
  }
  else {
    return "../gfx/mencus/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
  }
}
std::string getTileTexturePath(unsigned int uiTileID) {
  return "../gfx/tiles/Tile" + Ogre::StringConverter::toString(uiTileID) + ".png";
}
std::string getBackgroundTexturePath(std::string sName) {
  return "../gfx/background/" + sName + ".jpg";
}
