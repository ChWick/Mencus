#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "AnimatedSprite.hpp"

class CMap;

class CObject : public CAnimatedSprite {
public:
  enum EObjectTypes {
    OT_BOMB,
    OT_HEALTH_POTION,
    OT_MANA_POTION,
    OT_KEY,
    OT_SCRATCH,
    OT_TORCH,
    OT_FLAG,

    OT_COUNT
  };

  static Ogre::String getPreviewImageName(int iObjectType);
private:
  bool m_bIsPickable;
public:
  CObject(CMap &map,
	  const std::string &sID,
	  CEntity *pParent,
	  const Ogre::Vector2 &vPosition,
	  EObjectTypes eObjectType);

  CObject(CMap &map,
	  CEntity *pParent,
	  const tinyxml2::XMLElement *pElement);

  void update(Ogre::Real tpf);

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;

private:
  void constructor_impl();
};


#endif
