/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#include "XMLHelper.hpp"
#include "BoundingBox2d.hpp"

namespace XMLHelper {

  CBoundingBox2d BoundingBox2dAttribute(const tinyxml2::XMLElement *pElem,
					const Ogre::String &prefix) {

    assert(pElem);
    CBoundingBox2d out;
    if (!pElem->Attribute((prefix + "_pos").c_str())) {
      throw Ogre::Exception(0,
			    "Required attribute '" + Ogre::String(prefix + "_pos") + "' not found",
			    __FILE__);
    }
    if (!pElem->Attribute((prefix + "_size").c_str())) {
      throw Ogre::Exception(0,
			    "Required attribute '" + Ogre::String(prefix + "_size") + "' not found",
			    __FILE__);
    }
    out.setPosition(Ogre::StringConverter::parseVector2((prefix + "_pos").c_str()));
    out.setPosition(Ogre::StringConverter::parseVector2((prefix + "_size").c_str()));
    return out;

  }
  CBoundingBox2d BoundingBox2dAttribute(const tinyxml2::XMLElement *pElem,
					const CBoundingBox2d &bbDefault,
					const Ogre::String &prefix) {
    assert(pElem);
    CBoundingBox2d out(bbDefault);
    if (pElem->Attribute((prefix + "_pos").c_str())) {
      out.setPosition(Ogre::StringConverter::parseVector2((prefix + "_pos").c_str()));
    }
    if (pElem->Attribute((prefix + "_size").c_str())) {
      out.setPosition(Ogre::StringConverter::parseVector2((prefix + "_size").c_str()));
    }
    return out;
  }
  bool BoolAttribute(const tinyxml2::XMLElement *pElem,
		     const char *pLabel,
		     bool bDefault,
		     bool bRequired)
    {
    assert(pElem);
    assert(pLabel);
    if (pElem->Attribute(pLabel)) {
      std::string label = pElem->Attribute(pLabel);
      return label == "true" || label == "True" || label == "1";
    }
    else {
      if (bRequired) {
	throw Ogre::Exception(0,
			      "Required attribute '" + Ogre::String(pLabel) + "' not found",
			      __FILE__);
      }
      return bDefault;
    }
  }

  Ogre::Real RealAttribute(const tinyxml2::XMLElement *pElem,
			   const char *pLabel,
			   Ogre::Real fDefault,
			   bool bRequired) {
    if (pElem->Attribute(pLabel)) {
      return pElem->FloatAttribute(pLabel);
    }
    if (bRequired) {
      throw Ogre::Exception(0,
			    "Required attribute '" + Ogre::String(pLabel) + "' not found",
			    __FILE__);
    }
    return fDefault;
  }
  int IntAttribute(const tinyxml2::XMLElement *pElem,
		   const char *pLabel,
		   int iDefault) {
    if (pElem->Attribute(pLabel)) {
      return pElem->IntAttribute(pLabel);
    }
    return iDefault;
  }
  int IntAttribute(const tinyxml2::XMLElement *pElem,
		   const char *pLabel) {
    if (pElem->Attribute(pLabel)) {
      return pElem->IntAttribute(pLabel);
    }
    throw Ogre::Exception(0,
			  "Required attribute '" + Ogre::String(pLabel) + "' not found",
			  __FILE__);
  }

  Ogre::String Attribute(const tinyxml2::XMLElement *pElem,
			 const char *pLabel,
			 const Ogre::String &sDefault,
			 const  bool bRequired) {
    assert(pElem);
    assert(pLabel);
    if (pElem->Attribute(pLabel)) {return Ogre::String(pElem->Attribute(pLabel));}
    if (bRequired) {
      throw Ogre::Exception(0,
			    "Required attribute '" + Ogre::String(pLabel) + "' not found",
			    __FILE__);
    }
    return sDefault;
  }
  Ogre::Vector2 Vector2Attribute(const tinyxml2::XMLElement *pElem,
				 const Ogre::String &prefix,
				 const Ogre::Vector2 &vDefault,
				 bool bRequired) {
    return Ogre::Vector2(RealAttribute(pElem, (prefix + "x").c_str(), vDefault.x, bRequired),
			 RealAttribute(pElem, (prefix + "y").c_str(), vDefault.y, bRequired));
  }


  void SetAttribute(tinyxml2::XMLElement *pElem,
		    const char *pLabel,
		    const Ogre::Vector2 &vec) {
    pElem->SetAttribute((Ogre::String(pLabel) + "x").c_str(), vec.x);
    pElem->SetAttribute((Ogre::String(pLabel) + "y").c_str(), vec.y);
  }
  void SetAttribute(tinyxml2::XMLElement *pElem,
		    const char *pLabel,
		    Ogre::Real value) {
    pElem->SetAttribute(pLabel, value);
  }
  void SetAttribute(tinyxml2::XMLElement *pElem,
		    const CBoundingBox2d &bb,
		    const Ogre::String &prefix) {
    SetAttribute<Ogre::Vector2>(pElem, (prefix + "_pos").c_str(), bb.getPosition()); 
    SetAttribute<Ogre::Vector2>(pElem, (prefix + "_size").c_str(), bb.getSize());
  }
  template <>
  void SetAttribute<std::string>(tinyxml2::XMLElement *pElem,
				 const char *pLabel,
				 const std::string &value) {
    pElem->SetAttribute(pLabel, value.c_str());
  }
  template <>
  void SetAttribute<bool>(tinyxml2::XMLElement *pElem,
				 const char *pLabel,
				 const bool &value) {
    pElem->SetAttribute(pLabel, value ? "true" : "false");
  }
};
