#ifndef CAMERA_RESTRICTION
#define CAMERA_RESTRICTION

#include <OgreVector2.h>
#include <algorithm>

class CCameraRestriction {
public:
  enum ETypes {
    HORIZONTAL_RESTRICTION,
    VERTICAL_RESTRICTION
  };
private:
  const ETypes m_eRestrictionType;
  const float m_fLevel;
  const Ogre::Vector2 &m_vScreensize;

public:
  CCameraRestriction(const ETypes eRestrictionType, const Ogre::Vector2 &vScreensize, float fLevel)
    : m_eRestrictionType(eRestrictionType),
      m_fLevel(fLevel),
      m_vScreensize(vScreensize) {
  }

  CCameraRestriction(const CCameraRestriction &cr)
    : m_eRestrictionType(cr.m_eRestrictionType),
      m_fLevel(cr.m_fLevel),
      m_vScreensize(cr.m_vScreensize) {
  }

  void update(Ogre::Vector2 &vCameraPos, const Ogre::Vector2 &vPlayerCenter) {
    switch (m_eRestrictionType) {
    case HORIZONTAL_RESTRICTION:
      if (vPlayerCenter.y > m_fLevel) {
        vCameraPos.y = std::max<Ogre::Real>(vCameraPos.y, m_fLevel);
      }
      else {
        vCameraPos.y = std::min<Ogre::Real>(vCameraPos.y, m_fLevel);
      }
      break;
    case VERTICAL_RESTRICTION:
      if (vPlayerCenter.x > m_fLevel) {
        vCameraPos.x = std::max<Ogre::Real>(vCameraPos.x, m_fLevel);
      }
      else {
        vCameraPos.x = std::min<Ogre::Real>(vCameraPos.x, m_fLevel);
      }
      break;
    }
  }
};

#endif // CAMERA_RESTRICTION
