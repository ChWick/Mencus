#ifndef SHOT_HPP
#define SHOT_HPP

#include "AnimatedSprite.hpp"

class CMap;

class CShot : public CAnimatedSprite {
public:
  enum EShotDirections {
    SD_LEFT,
    SD_RIGHT,
  };
  enum EShotTypes {
    ST_BOLT,

    ST_COUNT,
  };

private:
  CMap *m_pMap;
  const EShotTypes m_eShotType;
  const bool m_bAffectedByGravity;
  Ogre::Vector2 m_vSpeed;
  const EShotDirections m_eShotDirection;
public:
  CShot(CMap *pMap,
	Ogre2dManager *pSpriteManager,
	const Ogre::Vector2 &vPosition,
	EShotTypes eShotType,
	EShotDirections eShotDirection);
  //! Lanches a shot with the given speed
  /** This speed will be multiplied with the default speed of the shot
   *  \param[in] vInitialSpeed The initial speed or direction vector
   */
  void launch(const Ogre::Vector2 &vInitialSpeed);

  void update(Ogre::Real tpf);
};

#endif
