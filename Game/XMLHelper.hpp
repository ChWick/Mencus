#ifndef _XML_HELPER_HPP_
#define _XML_HELPER_HPP_

#include <tinyxml2.h>
#include <assert.h>
#include <string>
#include <OgreString.h>
#include <OgreVector2.h>

namespace XMLHelper {
  bool BoolAttribute(tinyxml2::XMLElement *pElem,
		     const char *pLabel,
		     bool bDefault = true);

  Ogre::Real RealAttribute(tinyxml2::XMLElement *pElem,
			   const char *pLabel,
			   Ogre::Real fDefault = 0);

  Ogre::String Attribute(tinyxml2::XMLElement *pElem,
			 const char *pLabel,
			 const Ogre::String &sDefault = Ogre::StringUtil::BLANK);

  Ogre::Vector2 Vector2Attribute(tinyxml2::XMLElement *pElem,
				 const Ogre::String &prefix = Ogre::StringUtil::BLANK,
				 const Ogre::Vector2 &vDefault = Ogre::Vector2::ZERO);

  template <class T>
  T EnumAttribute(tinyxml2::XMLElement *pElem,
		  const char *pLabel,
		  T eDefault,
		  int iOffset = 0) {
    assert(pElem);
    assert(pLabel);
    if (!pElem->Attribute(pLabel)) {return eDefault;}
    return static_cast<T>(pElem->IntAttribute(pLabel) + iOffset);
  }

};





#endif
