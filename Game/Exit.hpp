#ifndef _EXIT_HPP_
#define _EXIT_HPP_

#include "DebugDefines.hpp"
#include <OgreString.h>
#include "BoundingBox2d.hpp"
#include <tinyxml2.h>

class CPlayer;
class CMap;

enum EExitTypes {
  EXIT_REGION,
  EXIT_ENEMY_DEATH,
};
class CExit {
public:
  static Ogre::String toString(EExitTypes et);
private:
  EExitTypes m_eExitType;
  CBoundingBox2d m_BoundingBox;
  Ogre::String m_sID;
public:
  static CExit newRegion(const CBoundingBox2d &bb) {return CExit(EXIT_REGION, bb, Ogre::StringUtil::BLANK);}
  static CExit newEnemyDeath(const Ogre::String &sID) {return CExit(EXIT_ENEMY_DEATH, CBoundingBox2d(), sID);}
public:
  CExit() 
    : m_eExitType(EXIT_REGION), m_BoundingBox(), m_sID() {
  }
  CExit(const CExit &src)
    : m_eExitType(src.m_eExitType), m_BoundingBox(src.m_BoundingBox),
      m_sID(src.m_sID) {
  }
  const CExit &operator=(const CExit &src) {
    m_eExitType = src.m_eExitType;
    m_BoundingBox = src.m_BoundingBox;
    m_sID = src.m_sID;
    return *this;
  }
private:
  CExit(EExitTypes eExitType, const CBoundingBox2d &bb, const Ogre::String &sID)
    : m_eExitType(eExitType), m_BoundingBox(bb), m_sID(sID) {
  }
public:
  bool isInExit(CPlayer *pPlayer, CMap *pMap);
#ifdef DEBUG_EXIT
  void debugDraw();
#endif
  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem) const;


  // getter setter
  EExitTypes getExitType() const {return m_eExitType;}
  void setExitType(EExitTypes eType) {m_eExitType = eType;}

  const CBoundingBox2d &getBoundingBox() const {return m_BoundingBox;}
  void setBoundingBox(const CBoundingBox2d &bb) {m_BoundingBox = bb;}

  const Ogre::String &getID() const {return m_sID;}
  void setID(const Ogre::String &id) {m_sID = id;}
};

#endif
