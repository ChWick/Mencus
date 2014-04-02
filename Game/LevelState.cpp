#include "LevelState.hpp"
#include "FileManager.hpp"
#include "OgreLogManager.h"
#include "XMLHelper.hpp"

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
SStatistics &CLevelState::get(const std::string &sFileName) {
  if (!m_bLoaded) {read();}

  for (auto &level : m_vLevelStatistics) {
    if (level.sLevelFileName == sFileName) {
      return level;
    }
  }
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
void CLevelState::read() {
  if (m_bLoaded) {
    return;
  }

  m_XMLDocument.LoadFile(CFileManager::getValidPath(LEVEL_STATE_FILE,
						    CFileManager::SL_INTERNAL).c_str());
  if (m_XMLDocument.Error()) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
						"Level save states could not be parsed");
    return;
  }

  XMLElement *pRoot = m_XMLDocument.FirstChildElement();
  assert(pRoot);

  for (XMLElement *pLevel = pRoot->FirstChildElement(); pLevel; pLevel = pLevel->NextSiblingElement()) {
    SStatistics stats(pLevel);
    m_vLevelStatistics.push_back(stats);
  }
}
void CLevelState::write() {
  XMLPrinter printer;
  m_XMLDocument.Accept(&printer);
  
  std::fstream os;
  if (!CFileManager::openFile(os,
			      LEVEL_STATE_FILE,
			      std::ios::trunc | std::ios::out,
			      CFileManager::SL_INTERNAL)) {
    os << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    os << printer.CStr();
  }
}
