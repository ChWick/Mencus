#ifndef SHOT_HPP
#define SHOT_HPP

#include "AnimatedSprite.hpp"

class CMap;
class CEnemy;

class CShot : public CAnimatedSprite {
public:
  enum EShotStates {
    SS_NONE,
    SS_LAUNCHED,
    SS_ENEMY_CAUGHT,
  };
  enum EShotAnimations {
    SA_DEFAULT = 0,
    SA_LAUNCHED,

    SA_COUNT
  };
  enum EShotDirections {
    SD_LEFT,
    SD_RIGHT,
  };
  enum EShotTypes {
    ST_BOLT,
    ST_BOMB,
    ST_SKULL,
    ST_COLUMN,

    ST_COUNT,
  };
  enum EDamages {
    DMG_PLAYER = 1,
    DMG_ENEMY  = 2,
    DMG_ALL    = 3,
  };

  static const Ogre::Real SHOT_DAMAGE[ST_COUNT];
private:
  const bool m_bAffectedByGravity;
  Ogre::Vector2 m_vSpeed;
  const EShotDirections m_eShotDirection;
  Ogre::Real m_fTimer;
  EShotStates m_eState;
  unsigned int m_uiDamages;
  CEnemy *m_pCatchedEnemy;
public:
  CShot(CMap &map,
	const Ogre::Vector2 &vCenter,
	EShotTypes eShotType,
	EShotDirections eShotDirection,
	unsigned int uiDmg = DMG_ALL);
  CShot(CMap &map,
	const tinyxml2::XMLElement *pElement);
  //! Lanches a shot with the given speed
  /** This speed will be multiplied with the default speed of the shot
   *  \param[in] vInitialSpeed The initial speed or direction vector
   *  \param[in] uiAnimationSequence New animation sequence on launch, if SA_COUNT class will handle on its own
   */
  void launch(const Ogre::Vector2 &vInitialSpeed, unsigned int uiNewAnimationSequence = SA_COUNT);

  void update(Ogre::Real tpf);

  void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;
protected:
  virtual void handleMessage(const CMessage &message);
private:
  void constructor_impl();
  void hit();
};

#endif
