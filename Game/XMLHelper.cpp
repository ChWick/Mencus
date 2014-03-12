#include "XMLHelper.hpp"

namespace XMLHelper {
  bool BoolAttribute(tinyxml2::XMLElement *pElem, const char *pLabel, bool bDefault)
    {
    assert(pElem);
    assert(pLabel);
    if (pElem->Attribute(pLabel)) {
      std::string label = pElem->Attribute(pLabel);
      return label == "true" || label == "True" || label == "1";
    }
    else {
      return bDefault;
    }
  }

  Ogre::Real RealAttribute(tinyxml2::XMLElement *pElem,
			   const char *pLabel,
			   Ogre::Real fDefault) {
    if (pElem->Attribute(pLabel)) {
      return pElem->FloatAttribute(pLabel);
    }
    return fDefault;
  }
  Ogre::String Attribute(tinyxml2::XMLElement *pElem, const char *pLabel, const Ogre::String &sDefault) {
    assert(pElem);
    assert(pLabel);
    if (pElem->Attribute(pLabel)) {return Ogre::String(pElem->Attribute(pLabel));}
    return sDefault;
  }
  Ogre::Vector2 Vector2Attribute(tinyxml2::XMLElement *pElem,
				 const Ogre::String &prefix,
				 const Ogre::Vector2 &vDefault) {
    return Ogre::Vector2(RealAttribute(pElem, (prefix + "x").c_str(), vDefault.x),
			 RealAttribute(pElem, (prefix + "y").c_str(), vDefault.y));
  }
};
