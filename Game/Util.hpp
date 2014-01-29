#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <Ogre.h>

template <unsigned int id>
std::string getEnemyTexturePath(std::string sName, int index = -1) {
  if (index < 0) {
    return "../gfx/enemies/enemy" + Ogre::StringConverter::toString(id) + "/" + sName + ".png";
  }
  return "../gfx/enemies/enemy" + Ogre::StringConverter::toString(id) + "/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
}
std::string getShotTexture(const std::string &sType, const std::string &sName, int index = -1);
std::string getSkullTexture(std::string sName, int index = -1);
std::string getBoltTexture(std::string sName, int index = -1);
std::string getHUDTexturePath(std::string sName);
std::string getBombTexture(std::string sName, int index = -1);
std::string getSwitchTexture(unsigned int uiSwitchID, bool on);
std::string getTextureInGfx(std::string sTextureName);
std::string getPlayerTexturePath(std::string sName, int index = -1);
std::string getTileTexturePath(unsigned int uiTileID);
std::string getBackgroundTexturePath(std::string sName);
std::string getOtherObjectsTexturePath(std::string sName);
std::string getTorchTexturePath(std::string sName, int index = -1);

#endif
