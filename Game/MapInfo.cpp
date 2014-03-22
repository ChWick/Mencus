#include "MapInfo.hpp"
#include <OgreResourceGroupManager.h>
#include <tinyxml2.h>
#include <OgreLogManager.h>
#include <XMLHelper.hpp>

using namespace tinyxml2;
using namespace XMLHelper;

CMapInfo::CMapInfo(const std::string &sFileName, const std::string &sResourceGroup)
  : m_sFileName(sFileName),
    m_eDifficulty(D_EASY) {

  Ogre::DataStreamPtr dataStream = Ogre::ResourceGroupManager::getSingleton().
    openResource(sFileName, sResourceGroup);
  if (dataStream.isNull()) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File " + sFileName + " not found!");
    return;
  }
  m_Document.Parse(dataStream->getAsString().c_str());
  
  constructor_impl();
}
CMapInfo::CMapInfo(const CMapInfoConstPtr src) 
  : m_bValid(src->m_bValid),
    m_sName(src->m_sName),
    m_sDescription(src->m_sDescription),
    m_sFileName(src->m_sFileName),
    m_eDifficulty(src->m_eDifficulty) {
  tinyxml2::XMLPrinter xmlprinter;
  src->m_Document.Accept(&xmlprinter);
  m_Document.Parse(xmlprinter.CStr());

  constructor_impl();
}
void CMapInfo::constructor_impl() {
  if (m_Document.Error()) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File could not be parsed as xml document");
    return;
  }

  XMLElement *pRoot = m_Document.FirstChildElement();
  if (!pRoot) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File has no root node");
    return;
  }
  m_eDifficulty = parseMapDifficlty(Attribute(pRoot, "difficulty", "unknown"));
  m_sName = Attribute(pRoot, "name", "unknown");
  m_sDescription = Attribute(pRoot, "description");

  m_bValid = true;
}
std::string CMapInfo::generateInfoText() const {
  return "Name: " + m_sName + "\nDifficulty: " + toString(m_eDifficulty) + "\nDescription: " + m_sDescription;
}
tinyxml2::XMLElement *CMapInfo::getEmptyRootNode() {
  m_Document.Clear();
  tinyxml2::XMLElement *pElem =  m_Document.NewElement("map");
  m_Document.InsertEndChild(pElem);

  return pElem;
}
