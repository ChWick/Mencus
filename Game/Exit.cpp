#include "Exit.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "DebugDrawer.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

Ogre::String CExit::toString(EExitTypes et) {
  switch (et) {
  case EXIT_REGION:
    return "region";
  case EXIT_ENEMY_DEATH:
    return "enemyDeath";
  }
  throw Ogre::Exception(0, "Unknown exit type", __FILE__);
}
bool CExit::isInExit(CPlayer *pPlayer, CMap *pMap) {
  switch (m_eExitType) {
  case EXIT_REGION:
    if (m_BoundingBox.collidesWith(pPlayer->getWorldBoundingBox()) != CCD_NONE) {
      return true;
    }
    break;
  case EXIT_ENEMY_DEATH:
    if (pMap->getEnemyById(m_sID) == NULL) {
      return true;
    }
    break;
  }
  return false;
}
void CExit::debugDraw() {
  switch (m_eExitType) {
  case EXIT_REGION:
    CDebugDrawer::getSingleton().draw(m_BoundingBox);
    break;
  default:
    break;
  }
}
void CExit::writeToXMLElement(tinyxml2::XMLElement *pElem) const {
  pElem->SetAttribute("type", toString(m_eExitType).c_str());
  SetAttribute(pElem, "pos", m_BoundingBox.getPosition());
  SetAttribute(pElem, "size", m_BoundingBox.getSize());
  pElem->SetAttribute("id", m_sID.c_str());
}
