#ifndef LINK_HPP
#define LINK_HPP

#include <Ogre.h>
#include <string>

class CLink {
public:
  enum ELinkDirection {
    LD_FIRST_TO_SECOND    = 1,
    LD_SECOND_TO_FIRST    = 2,
    LD_BOTH               = 3
  };

  static std::string toString(ELinkDirection eLD) {
    switch (eLD) {
    case LD_FIRST_TO_SECOND:
      return "first_to_second";
    case LD_SECOND_TO_FIRST:
      return "second_to_first";
    default:
      return "both";
    }
  }
  static ELinkDirection parseLinkDirection(const std::string &str) {
    if (str == "first_to_second") {return LD_FIRST_TO_SECOND;}
    if (str == "second_to_first") {return LD_SECOND_TO_FIRST;}
    if (str == "both") {return LD_BOTH;}
    throw Ogre::Exception(0, "Invalid direction in link: " + str, __FILE__);
  }
private:
  unsigned int m_uiFirstX;
  unsigned int m_uiFirstY;
  unsigned int m_uiSecondX;
  unsigned int m_uiSecondY;
  const Ogre::String m_sID;
  bool m_bActivated;
  ELinkDirection m_eLinkDirection;
public:
  CLink(unsigned int uiFirstX, unsigned int uiFirstY,
	unsigned int uiSecondX, unsigned int uiSecondY,
	ELinkDirection eLinkDirection = LD_BOTH,
	const Ogre::String &sID = Ogre::StringUtil::BLANK,
	bool bActivated = true)
    : m_uiFirstX(uiFirstX), m_uiFirstY(uiFirstY),
      m_uiSecondX(uiSecondX), m_uiSecondY(uiSecondY),
      m_sID(sID),
      m_bActivated(bActivated),
      m_eLinkDirection(eLinkDirection) {
  }
  CLink(const CLink &link)
    : m_uiFirstX(link.m_uiFirstX), m_uiFirstY(link.m_uiFirstY),
      m_uiSecondX(link.m_uiSecondX), m_uiSecondY(link.m_uiSecondY),
      m_sID(link.m_sID),
      m_bActivated(link.m_bActivated),
      m_eLinkDirection(link.m_eLinkDirection) {
  }

  unsigned int getFirstX() const {return m_uiFirstX;}
  unsigned int getFirstY() const {return m_uiFirstY;}

  unsigned int getSecondX() const {return m_uiSecondX;}
  unsigned int getSecondY() const {return m_uiSecondY;}


  unsigned int &getFirstX() {return m_uiFirstX;}
  unsigned int &getFirstY() {return m_uiFirstY;}

  unsigned int &getSecondX() {return m_uiSecondX;}
  unsigned int &getSecondY() {return m_uiSecondY;}

  const Ogre::String &getID() const {return m_sID;}

  bool isActivated() const {return m_bActivated;}
  void setActivated(bool b) {m_bActivated = b;}
  bool &getActivated() {return m_bActivated;}

  ELinkDirection getLinkDirection() const {return m_eLinkDirection;}

  std::string toString() const {
    return "Link with id (" + m_sID + ") from (" + Ogre::StringConverter::toString(m_uiFirstX) +
      ", " + Ogre::StringConverter::toString(m_uiFirstY) + ") to (" +
      Ogre::StringConverter::toString(m_uiSecondX) + ", " +
      Ogre::StringConverter::toString(m_uiSecondY) + ")";
  }
};

#endif
