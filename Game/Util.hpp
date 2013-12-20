#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>

std::string getBombTexture(std::string sName, int index = -1);
std::string getSwitchTexture(unsigned int uiSwitchID, bool on);
std::string getTextureInGfx(std::string sTextureName);
std::string getPlayerTexturePath(std::string sName, int index = -1);
std::string getTileTexturePath(unsigned int uiTileID);
std::string getBackgroundTexturePath(std::string sName);


#endif
