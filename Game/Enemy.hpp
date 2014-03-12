#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "AnimatedSprite.hpp"
#include "HitableObject.hpp"
#include "BarIndicator.hpp"

class CMap;

class CEnemy : public CAnimatedSprite, public CHitableObject {
public:
  enum EAttackType {
    AT_MELEE = 1,
    AT_RANGED = 2,
    AT_ALL = 3
  };
  enum EEnemyTypes {
    ET_GREEN_MONSTER,
    ET_KNIGHT,
    ET_BEAR,
    ET_GHOST,
    ET_DARK_MAGICAN,
    ET_SANTA,
    ET_COUNT,
  };
  enum EAttackSequences {
    AS_WALK_LEFT,
    AS_WALK_RIGHT,
    AS_ATTACK_LEFT,
    AS_ATTACK_RIGHT,
    AS_JUMP_LEFT,
    AS_JUMP_RIGHT,
    AS_RANGED_ATTACK_LEFT,
    AS_RANGED_ATTACK_RIGHT,

    AS_COUNT
  };
private:
  const EEnemyTypes m_eEnemyType;
  CMap &m_Map;
  Ogre::Vector2 m_vSpeed;
  bool m_bOnGround;
  CBarIndicator m_HPBar;
  const bool m_bJumps;
  const Ogre::String m_sID;
  Ogre::Vector2 m_vExternalForce;
  bool m_bStunned;
  bool m_bAtLeastOneDamageDone;
public:
  CEnemy(CMap &map,
    const Ogre::Vector2 &vPosition,
    EEnemyTypes eEnemyType,
    Ogre::Real fDirection,
    Ogre::Real fHitpoints,
    bool bJumps,
    const Ogre::String &sID);
  CEnemy(tinyxml2::XMLElement *pElem);

  void update(Ogre::Real tpf);
  void render(Ogre::Real tpf);

  void addExternalForce(const Ogre::Vector2 &vForce) {m_vExternalForce += vForce;}

  const Ogre::String &getID() const {return m_sID;}
  EEnemyTypes getType() const {return m_eEnemyType;}

  void setStunned(bool bStunned) {m_bStunned = bStunned;}
  bool isStunned() const {return m_bStunned;}

  bool mayJump() const {return m_bJumps;}

  Ogre::Real getDirection() const {return m_vSpeed.x > 0 ? 1 : -1;}
  const Ogre::Vector2 &getSpeed() const {return m_vSpeed;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem);

protected:
  void damageTakenCallback();
  void killedByDamageCallback();
  void animationTextureChangedCallback(unsigned int uiOldText, unsigned uiNewText);
  bool readyForWalking();
private:
  void setup();
  void updateKI();
};

#endif
