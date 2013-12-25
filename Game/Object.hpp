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

    OT_COUNT
  };
private:
  const EObjectTypes m_eObjectType;
  CMap &m_Map;
public:
  CObject(CMap &map,
	  const Ogre::Vector2 &vPosition,
	  EObjectTypes eObjectType);

  void update(Ogre::Real tpf);

};


#endif
