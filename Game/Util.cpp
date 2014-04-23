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
CSpriteTexture getTileTexture(unsigned int uiTileID) {
  // slightly smaller swaped size, since elsewise stripes on android occur
  Ogre::Vector2 vTopLeft(static_cast<int>(uiTileID % 16) / 16.f,
			 static_cast<int>(uiTileID / 16) / 16.0 + 0.062449);
  Ogre::Vector2 vSize(0.0625, -0.062448);
  return CSpriteTexture(Ogre::TextureManager::getSingleton().
			getByName("tiles/Atlas.png", "Game"),
			vTopLeft,
			vTopLeft + vSize,
			Ogre::Vector2(1, -1));
}
CSpriteTexture getBackgroundTexture(std::string sName) {
  std::string path("background/" + sName + ".jpg");
  return CSpriteTexture(Ogre::TextureManager::getSingleton().
			getByName(path));
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
std::string getFlagTexturePath(std::string sName, int index) {
  if (index < 0) {
    return "objects/flag/" + sName + ".png";
  }
  return "objects/flag/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
}
