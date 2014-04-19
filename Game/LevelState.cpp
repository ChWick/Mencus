#include "LevelState.hpp"
#include "FileManager.hpp"
#include "OgreLogManager.h"
#include "XMLHelper.hpp"
#include <OgreException.h>

using namespace tinyxml2;
using namespace XMLHelper;

const std::string LEVEL_STATE_FILE("level_state.xml");

std::vector<SStatistics> CLevelState::m_vLevelStatistics;
bool CLevelState::m_bLoaded(false);
XMLDocument CLevelState::m_XMLDocument;

bool CLevelState::has(const std::string &sFileName) {
  if (!m_bLoaded) {read();}

  for (auto &level : m_vLevelStatistics) {
    if (level.sLevelFileName == sFileName) {
      return true;
    }
  }
  return false;
}
SStatistics &CLevelState::get(const std::string &sFileName,
			      bool createIfNotExisting) {
  if (!m_bLoaded) {read();}

  for (auto &level : m_vLevelStatistics) {
    if (level.sLevelFileName == sFileName) {
      return level;
    }
  }

  if (createIfNotExisting) {
    SStatistics s(sFileName);
    add(s);
    return m_vLevelStatistics.back();
  }

  throw Ogre::Exception(0, sFileName + " not found in level states, check with 'has' function whether it exists", __FILE__);
}
bool CLevelState::levelAccomplished(const std::string &sFileName) {
  if (!m_bLoaded) {read();}

  for (auto &level : m_vLevelStatistics) {
    if (level.sLevelFileName == sFileName) {
      return level.eMissionState == MS_ACCOMPLISHED;
    }
  }
  return false;
}
void CLevelState::add(const SStatistics &stats) {
  if (has(stats.sLevelFileName)) {
    get(stats.sLevelFileName) = stats;
    stats.writeToXML(getXMLElement(stats.sLevelFileName));
  }
  else {
    m_vLevelStatistics.push_back(stats);
    XMLElement *pXMLElem = m_XMLDocument.NewElement("level");
    m_XMLDocument.FirstChildElement()->InsertEndChild(pXMLElem);
    stats.writeToXML(pXMLElem);
  }
  write();
}

tinyxml2::XMLElement *CLevelState::getXMLElement(const std::string &sFileName) {
  XMLElement *pRoot = m_XMLDocument.FirstChildElement();
  assert(pRoot);

  for (XMLElement *pLevel = pRoot->FirstChildElement(); pLevel; pLevel = pLevel->NextSiblingElement()) {
    if (pLevel->Attribute("level") == sFileName) {
      return pLevel;
    }
  }
  return NULL;
}
void CLevelState::read() {
  if (m_bLoaded) {
    return;
  }
  Ogre::LogManager::getSingleton().logMessage("Reading level states");

  m_XMLDocument.LoadFile(CFileManager::getValidPath(LEVEL_STATE_FILE,
						    CFileManager::SL_INTERNAL).c_str());
  if (m_XMLDocument.Error()) {
    Ogre::LogManager::getSingleton().logMessage("Level save states could not be parsed, creating root node");
    m_XMLDocument.InsertEndChild(m_XMLDocument.NewElement("level_states"));
  }

  XMLElement *pRoot = m_XMLDocument.FirstChildElement();
  if (!pRoot) {
    // empty file
    Ogre::LogManager::getSingleton().logMessage("Empty level save states, creating root node");
    pRoot = m_XMLDocument.NewElement("level_states");
    m_XMLDocument.InsertEndChild(pRoot);
  }

  for (XMLElement *pLevel = pRoot->FirstChildElement(); pLevel; pLevel = pLevel->NextSiblingElement()) {
    SStatistics stats(pLevel);
    // this is for compability issue
    if (stats.sLevelFileName.rfind(".xml") != std::string::npos) {
      // cut
      stats.sLevelFileName = stats.sLevelFileName.substr(0, stats.sLevelFileName.size() - 4);
      pLevel->SetAttribute("level", stats.sLevelFileName.c_str());
    }

    m_vLevelStatistics.push_back(stats);
  }


  Ogre::LogManager::getSingleton().logMessage("Read " + Ogre::StringConverter::toString(m_vLevelStatistics.size()) + " levels");
  m_bLoaded = true;
}
void CLevelState::write() {
  XMLPrinter printer;
  m_XMLDocument.Accept(&printer);
  
  std::fstream os;
  if (CFileManager::openFile(os,
			     LEVEL_STATE_FILE,
			     std::ios::trunc | std::ios::out,
			     CFileManager::SL_INTERNAL)) {
    //os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    os << printer.CStr();
  }
}
