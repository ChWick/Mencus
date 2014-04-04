#ifndef _CHANGE_TILE_EVENT_HPP_
#define _CHANGE_TILE_EVENT_HPP_

#include "Event.hpp"
#include <OgreStringConverter.h>

class CChangeTileEvent : public CEvent {
private:
  unsigned int m_uiTileType;
  unsigned int m_uiOldTileType;
  unsigned int m_uiTilePosX;
  unsigned int m_uiTilePosY;
public:
  CChangeTileEvent(CMap &map);
  CChangeTileEvent(CMap &map, const tinyxml2::XMLElement *pElem);

  void init();

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle);

  unsigned int getTileType() const {return m_uiTileType;}
  unsigned int getOldTileType() const {return m_uiOldTileType;}
  unsigned int getTilePosX() const {return m_uiTilePosX;}
  unsigned int getTilePosY() const {return m_uiTilePosY;}

  unsigned int &getTileType() {return m_uiTileType;}
  unsigned int &getOldTileType() {return m_uiOldTileType;}
  unsigned int &getTilePosX() {return m_uiTilePosX;}
  unsigned int &getTilePosY() {return m_uiTilePosY;}

  std::string toString() const {
    return "ChangeTileEvent at (" + Ogre::StringConverter::toString(m_uiTilePosX)
      + ", " + Ogre::StringConverter::toString(m_uiTilePosY)
      + ") Tile " + Ogre::StringConverter::toString(m_uiTileType)
      + "->" + Ogre::StringConverter::toString(m_uiOldTileType);
  }
};

#endif
