#ifndef _REGION_HPP_
#define _REGION_HPP_

#include "Entity.hpp"
#include "DebugDrawer.hpp"
#include "DebugDefines.hpp"

class CRegion : public CEntity {
public:
  CRegion(CMap &map, CEntity *pParent, const tinyxml2::XMLElement *pElem)
    : CEntity(map, pParent, pElem) {
    m_bbRelativeBoundingBox.setSize(m_vSize);
  }
  
  void render(Ogre::Real tpf) {
#ifdef DEBUG_REGION
    CDebugDrawer::getSingleton().draw(getWorldBoundingBox());
#endif
  }

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
    CEntity::writeToXMLElement(pElem, eStyle);
  }
};
#endif
