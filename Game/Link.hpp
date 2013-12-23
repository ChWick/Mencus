#ifndef LINK_HPP
#define LINK_HPP

#include <Ogre.h>
#include <string>

class CLink {
private:
  unsigned int m_uiFirstX;
  unsigned int m_uiFirstY;
  unsigned int m_uiSecondX;
  unsigned int m_uiSecondY;

public:
  CLink(unsigned int uiFirstX, unsigned int uiFirstY,
	unsigned int uiSecondX, unsigned int uiSecondY) 
    : m_uiFirstX(uiFirstX), m_uiFirstY(uiFirstY),
      m_uiSecondX(uiSecondX), m_uiSecondY(uiSecondY) {
  }
  CLink(const CLink &link)
    : m_uiFirstX(link.m_uiFirstX), m_uiFirstY(link.m_uiFirstY),
      m_uiSecondX(link.m_uiSecondX), m_uiSecondY(link.m_uiSecondY) {
  }

  unsigned int getFirstX() const {return m_uiFirstX;}
  unsigned int getFirstY() const {return m_uiFirstY;}

  unsigned int getSecondX() const {return m_uiSecondX;}
  unsigned int getSecondY() const {return m_uiSecondY;}

  std::string toString() const {
    return "Link from (" + Ogre::StringConverter::toString(m_uiFirstX) +
      ", " + Ogre::StringConverter::toString(m_uiFirstY) + ") to (" +
      Ogre::StringConverter::toString(m_uiSecondX) + ", " +
      Ogre::StringConverter::toString(m_uiSecondY) + ")";
  }
};

#endif
