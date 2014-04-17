#ifndef _XMLRESOURCES_MANAGER_HPP_
#define _XMLRESOURCES_MANAGER_HPP_

#include <map>
#include <tinyxml2.h>
#include <OgrePlatform.h>
#include <OgreException.h>
#include <OgreResourceGroupManager.h>
#include <CEGUI/String.h>

namespace XMLResources {
  class CManager {
  public:
    static std::string LANGUAGE_CODE;
  private:

    std::map<std::string, std::string> m_lStringResources;
    const std::string m_sResourceGroup;
  public:
    CManager(const std::string &sResourceGroup, bool bLoadOnCreate = true)
      : m_sResourceGroup(sResourceGroup) {
      if (bLoadOnCreate) {
	loadLanguage();
      }
    }
    
    const std::string &getString(const std::string &id) {return m_lStringResources.at(id);}
    const CEGUI::String getCEGUIString(const std::string &id) {
      return reinterpret_cast<const CEGUI::utf8*>(m_lStringResources.at(id).c_str());
    }
    void loadLanguage() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
      parse("language/values/strings.xml");
      if (LANGUAGE_CODE.length() > 0) {
	parse("language/values-" + LANGUAGE_CODE + "/strings.xml");
      }
#else
      parse("../language/values/strings.xml");
      if (LANGUAGE_CODE.length() > 0) {
	parse("../language/values-" + LANGUAGE_CODE + "/strings.xml");
      }
#endif
    }
  private:
    void parse(const Ogre::String &path) {
      using namespace tinyxml2;

      XMLDocument doc;
      if (m_sResourceGroup.size() > 0) {
	Ogre::DataStreamPtr pData(Ogre::ResourceGroupManager::getSingleton().openResource(path, m_sResourceGroup, false));
	if (pData.isNull()) {
	  throw Ogre::Exception(0, path + " not found in resource group " + m_sResourceGroup, __FILE__);
	}

	doc.Parse(pData->getAsString().c_str());
      }
      else {
	doc.LoadFile(path.c_str());
      }
      
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

  extern CManager GLOBAL;
};

#endif
