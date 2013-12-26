#ifndef BOUNDING_BOX_2D
#define BOUNDING_BOX_2D

#include <Ogre.h>
#include <cmath>
#include "CollisionDefines.hpp"

using namespace std;

class CBoundingBox2d {
private:
  Ogre::Vector2 m_vPosition;
  Ogre::Vector2 m_vSize;
public:
  CBoundingBox2d(const Ogre::Vector2 &vPosition = Ogre::Vector2::ZERO,
		 const Ogre::Vector2 &vSize = Ogre::Vector2::ZERO)
    :
    m_vPosition(vPosition),
    m_vSize(vSize) {
  }
  CBoundingBox2d(const CBoundingBox2d &src)
  :
  m_vPosition(src.m_vPosition),
    m_vSize(src.m_vSize) {
  }
  const CBoundingBox2d &operator=(const CBoundingBox2d &src) {
    m_vPosition = src.m_vPosition;
    m_vSize = src.m_vSize;
    return *this;
  }

  const Ogre::Vector2 &getPosition() const {return m_vPosition;}
  const Ogre::Vector2 &getSize() const {return m_vSize;}
  const Ogre::Vector2 &getCenter() const {return m_vPosition + m_vSize / 2;}

  void setPosition(const Ogre::Vector2 &vPosition) {m_vPosition = vPosition;}
  void setSize(const Ogre::Vector2 &vSize) {m_vSize = vSize;}

  CBoundingBox2d translate(const Ogre::Vector2 &vTranslation) const {
    return CBoundingBox2d(m_vPosition + vTranslation, m_vSize);
  }
  bool contains(const Ogre::Vector2 &vPoint) const {
    if ( vPoint.x > m_vPosition.x + m_vSize.x) {return false;}
    if ( vPoint.x < m_vPosition.x ) { return false; }
    if ( vPoint.y > m_vPosition.y + m_vSize.y) {return false; }
    if ( vPoint.y < m_vPosition.y ) { return false; }

    return true;
  }
  unsigned int collidesWith(const CBoundingBox2d &other, CBoundingBox2d *pCollisionBox = NULL) const {
    if (m_vPosition.x + m_vSize.x < other.m_vPosition.x) {return CCD_NONE;}
    if (m_vPosition.x > other.m_vPosition.x + other.m_vSize.x) {return CCD_NONE;}
    if (m_vPosition.y + m_vSize.y < other.m_vPosition.y) {return CCD_NONE;}
    if (m_vPosition.y > other.m_vPosition.y + other.m_vSize.y) {return CCD_NONE;}

    if (pCollisionBox) {
      pCollisionBox->m_vPosition.x = max(m_vPosition.x, other.m_vPosition.x);
      pCollisionBox->m_vPosition.y = max(m_vPosition.y, other.m_vPosition.y);

      Ogre::Vector2 vTopRight = Ogre::Vector2(min(m_vPosition.x + m_vSize.x, other.m_vPosition.x + other.m_vSize.x), min(m_vPosition.y + m_vSize.y, other.m_vPosition.y + other.m_vSize.y));
      pCollisionBox->m_vSize = vTopRight - pCollisionBox->m_vPosition;
    }

    unsigned int uiColldir = CCD_NONE;
    if (collidesWith(other.m_vPosition + other.m_vSize, CLD_VERTICAL)) {uiColldir |= CCD_LEFT;}
    if (collidesWith(other.m_vPosition + other.m_vSize, CLD_HORIZONTAL)) {uiColldir |= CCD_BOTTOM;}
    if (collidesWith(other.m_vPosition, CLD_VERTICAL)) {uiColldir |= CCD_RIGHT;}
    if (collidesWith(other.m_vPosition, CLD_HORIZONTAL)) {uiColldir |= CCD_TOP;}

    return uiColldir;
  }
  bool collidesWith(const Ogre::Vector2 &vOrigin, ECollisionLineDirections eCLD) const {
    if (eCLD == CLD_VERTICAL) {
      return vOrigin.x >= m_vPosition.x && vOrigin.x <= m_vPosition.x + m_vSize.x;
    }
    return vOrigin.y >= m_vPosition.y && vOrigin.y <=m_vPosition.y + m_vSize.y;
  }
};

#endif
