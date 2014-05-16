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

#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <Ogre.h>
#include "SpriteTexture.hpp"

template <unsigned int id>
std::string getEnemyTexturePath(std::string sName, int index = -1) {
  if (index < 0) {
    return "enemies/enemy" + Ogre::StringConverter::toString(id) + "/" + sName + ".png";
  }
  return "enemies/enemy" + Ogre::StringConverter::toString(id) + "/" + sName + "_" + Ogre::StringConverter::toString(index) + ".png";
}
std::string getShotTexture(const std::string &sType, const std::string &sName, int index = -1);
std::string getSkullTexture(std::string sName, int index = -1);
std::string getBoltTexture(std::string sName, int index = -1);
std::string getColumnTexture(std::string sName, int index = -1);
std::string getHUDTexturePath(std::string sName);
std::string getBombTexture(std::string sName, int index = -1);
std::string getSwitchTexture(unsigned int uiSwitchID, bool on);
std::string getTextureInGfx(std::string sTextureName);
std::string getPlayerTexturePath(std::string sName, int index = -1);
std::string getTileTexturePath(unsigned int uiTileID);
CSpriteTexture getTileTexture(unsigned int uiTileID);
CSpriteTexture getBackgroundTexture(std::string sName);
std::string getOtherObjectsTexturePath(std::string sName);
std::string getTorchTexturePath(std::string sName, int index = -1);
std::string getFlagTexturePath(std::string sName, int index = -1);

#endif
