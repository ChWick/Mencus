#include "Manager.hpp"
#include "FileManager.hpp"

namespace XMLResources {
  std::string CManager::LANGUAGE_CODE;

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  CManager GLOBAL("", "language/", false);
#else
  CManager GLOBAL("", "../language/", false);
#endif

  void CManager::parse(const Ogre::String &path) {
    using namespace tinyxml2;
    
    XMLDocument doc;
    Ogre::DataStreamPtr pData;
    if (m_sResourceGroup.length() == 0) {
      pData = CFileManager::openDataStream(path);
    }
    else {
      pData = Ogre::ResourceGroupManager::getSingleton().openResource(path, m_sResourceGroup, false);
    }

    if (pData.isNull()) {
      throw Ogre::Exception(0, path + " not found in resource group " + m_sResourceGroup, __FILE__);
    }

    doc.Parse(pData->getAsString().c_str());
      
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
