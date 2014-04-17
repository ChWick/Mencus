#ifndef _XMLRESOURCES_MANAGER_HPP_
#define _XMLRESOURCES_MANAGER_HPP_

#include <map>
#include <tinyxml2.h>
#include <OgrePlatform.h>
#include <OgreException.h>

namespace XMLResources {
  class CManager {
  private:
    static std::map<std::string, std::string> m_lStringResources;

  public:
    static const std::string &getString(const std::string &id) {return m_lStringResources.at(id);}
    static void loadLanguage(const Ogre::String &sCode) {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
      parse("language/values/strings.xml");
      if (sCode.length() > 0) {
	parse("language/values-" + sCode + "/strings.xml");
      }
#else
      parse("../language/values/strings.xml");
      if (sCode.length() > 0) {
	parse("../language/values-" + sCode + "/strings.xml");
      }
#endif
    }
  private:
    static void parse(const Ogre::String &path) {
      using namespace tinyxml2;

      XMLDocument doc;
      doc.LoadFile(path.c_str());
      
      if (doc.Error()) {
	throw Ogre::Exception(doc.ErrorID(), doc.GetErrorStr1(), __FILE__);
      }
      
      XMLElement *pResourcesElem(doc.FirstChildElement());
      for (XMLElement *pElem = pResourcesElem->FirstChildElement();
	   pElem;
	   pElem = pElem->NextSiblingElement()) {
	if (strcmp(pElem->Value(), "string") == 0) {
	  m_lStringResources[pElem->Attribute("name")] = pElem->GetText();
	}
      }
    }
  };
};

#endif
