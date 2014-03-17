#include "MapInfo.hpp"
#include <OgreResourceGroupManager.h>
#include <tinyxml2.h>
#include <OgreLogManager.h>
#include <XMLHelper.hpp>

using namespace tinyxml2;
using namespace XMLHelper;

CMapInfo::CMapInfo(const std::string &sFileName, const std::string &sResourceGroup) {
  Ogre::DataStreamPtr dataStream = Ogre::ResourceGroupManager::getSingleton().
    openResource(sFileName, sResourceGroup);
  if (dataStream.isNull()) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File " + sFileName + " not found!");
    return;
  }
  m_Document.Parse(dataStream->getAsString().c_str());
  if (m_Document.Error()) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File " + sFileName + " could not be parsed as xml document");
    return;
  }

  XMLElement *pRoot = m_Document.FirstChildElement();
  if (!pRoot) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File " + sFileName + " has no root node");
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
