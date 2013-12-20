#ifndef BOUNDING_BOX_2D
#define BOUNDING_BOX_2D

#include <Ogre.h>
#include <cmath>

using namespace std;

class CBoundingBox2d {
private:
  Ogre::Vector2 m_vPosition;
  Ogre::Vector2 m_vSize;
public:
  CBoundingBox2d(const Ogre::Vector2 &vPosition, const Ogre::Vector2 &vSize)
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

  void setPosition(const Ogre::Vector2 &vPosition) {m_vPosition = vPosition;}
  void setSize(const Ogre::Vector2 &vSize) {m_vSize = vSize;}

  CBoundingBox2d translate(const Ogre::Vector2 &vTranslation) const {
    return CBoundingBox2d(m_vPosition + vTranslation, m_vSize);
  }

  bool collidesWith(const CBoundingBox2d &other, CBoundingBox2d *pCollisionBox = NULL) {
    if (m_vPosition.x + m_vSize.x < other.m_vPosition.x) {return false;}
    if (m_vPosition.x > other.m_vPosition.x + other.m_vSize.x) {return false;}
    if (m_vPosition.y + m_vSize.y < other.m_vPosition.y) {return false;}
    if (m_vPosition.y > other.m_vPosition.y + other.m_vSize.y) {return false;}
    
    if (pCollisionBox) {
      pCollisionBox->m_vPosition.x = max(m_vPosition.x, other.m_vPosition.x);
      pCollisionBox->m_vPosition.y = max(m_vPosition.y, other.m_vPosition.y);

      Ogre::Vector2 vTopLeft = Ogre::Vector2(max(m_vPosition.x + m_vSize.x, other.m_vPosition.x + other.m_vSize.x), max(m_vPosition.y + m_vSize.y, other.m_vPosition.y + m_vSize.y));
      pCollisionBox->m_vSize = vTopLeft - pCollisionBox->m_vPosition;
    }
    return true;
  }
};

#endif
