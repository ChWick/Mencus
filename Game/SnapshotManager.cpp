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

#include "SnapshotManager.hpp"
#include "GameState.hpp"
#include "Screenplay.hpp"
#include "FileManager.hpp"
#include "MapInfo.hpp"
#include "MapPack.hpp"
#include "XMLHelper.hpp"
#include <fstream>
#include <CEGUI/CEGUI.h>

using namespace std;
using namespace XMLHelper;
 
template<> CSnapshotManager* Ogre::Singleton<CSnapshotManager>::msSingleton = 0;

CSnapshotManager* CSnapshotManager::getSingletonPtr(void)
{
  return msSingleton;
}
CSnapshotManager& CSnapshotManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

CSnapshotManager::~CSnapshotManager() {
  for (auto pSnapshot : m_vSnapshots) {
    delete pSnapshot;
  }
}
const CSnapshot &CSnapshotManager::makeBackupSnapshot() {
  Ogre::LogManager::getSingleton().logMessage("Making backup snapshot");
  m_vSnapshots.push_back(new CSnapshot());
  return makeSnapshot("backup.xml", *m_vSnapshots.back());
}
const CSnapshot &CSnapshotManager::makeSnapshot(CSnapshot *pSnapshotOrig) {
  CSnapshot *pSnapshot = pSnapshotOrig;
  if (!pSnapshotOrig) {
    pSnapshot = new CSnapshot();
    m_vSnapshots.push_back(pSnapshot);
  }
  
  return makeSnapshot("snapshot.xml", *pSnapshot);
}
const CSnapshot &CSnapshotManager::makeSnapshot(const Ogre::String &name, CSnapshot &snapshot) {
  snapshot.setGameState(CGameState::getSingleton().getCurrentGameState());

  tinyxml2::XMLDocument &doc(snapshot.getXMLDocument());
  tinyxml2::XMLElement *pElem = doc.NewElement("snapshot");
  pElem->SetAttribute("version", SNAPSHOT_VERSION);
  pElem->SetAttribute("game_state", snapshot.getGameState());
  doc.InsertEndChild(pElem);

  switch (snapshot.getGameState()) {
  case CGameState::GS_AD:
    {
      pElem->SetAttribute("map_name", CGameState::getSingleton().getMapPack()->getMapName().c_str());
    }
    break;
  case CGameState::GS_GAME:
    {
      CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
      tinyxml2::XMLElement *pScreenplayElem = doc.NewElement("screenplay");
      pElem->InsertEndChild(pScreenplayElem);
      pScreenplay->writeToXMLElement(pScreenplayElem, OS_FULL);
    }
    break;
  case CGameState::GS_STATISTICS:
    {
      pElem->SetAttribute("map_name", CGameState::getSingleton().getMapPack()->getMapName().c_str());
      tinyxml2::XMLElement *pStatElem = doc.NewElement("statistics");
      pElem->InsertEndChild(pStatElem);
      CGameState::getSingleton().getStatistics().writeToXMLElement(pStatElem);
    }
  default:
    // not supported yet... state is not saved
    break;
  }
  
  //Ogre::LogManager::getSingleton().logMessage("Snapshot crated");
  // output
  // ============================================================
  std::fstream outputfile;
  if (CFileManager::openFile(outputfile, name, std::fstream::in | std::fstream::out | std::fstream::trunc, CFileManager::SL_INTERNAL)) {
    cout << name << " " << snapshot.getGameState() << endl;
    tinyxml2::XMLPrinter xmlprinter;
    snapshot.getXMLDocument().Accept(&xmlprinter);
    
    outputfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    outputfile << xmlprinter.CStr();
    cout << xmlprinter.CStr() << endl;
    outputfile.close();
  }

  return snapshot;
}
bool CSnapshotManager::loadBackupSnapshot() {
  CSnapshot *pSnapshot = NULL;
  try {
    pSnapshot = new CSnapshot(CFileManager::getValidPath("backup.xml", CFileManager::SL_INTERNAL).c_str());
    // delete snapshot file (if there is an error in the file, then on second start, should be in normal main menu
    CFileManager::deleteFile("backup.xml", CFileManager::SL_INTERNAL);
  }
  catch (const Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, e.getFullDescription());
    delete pSnapshot;
    pSnapshot = NULL;
    return false;
  }

  m_vSnapshots.push_back(pSnapshot);
  loadFromSnapshot(*pSnapshot);
  return true;
}
void CSnapshotManager::loadFromSnapshot(const CSnapshot &snapshot) {
  try {
    CGameState::EGameStates eGameStateToChangeTo = snapshot.getGameState();
    const tinyxml2::XMLElement *pSnapshotElem =
      snapshot.getXMLDocument().FirstChildElement("snapshot");
    
    Ogre::LogManager::getSingleton().logMessage("Loading snapshot...");

    if (snapshot.getGameState() == CGameState::GS_GAME) {
      // no ad when loading from savesate
      CGameState::getSingleton().setAdShown(true);
    }
    else if (snapshot.getGameState() == CGameState::GS_AD) {
      // no ad when loading from savesate
      CGameState::getSingleton().setAdShown(true);
      std::shared_ptr<CMapPack> pMapPack(new CMapPack(Attribute(pSnapshotElem,
								"map_name").c_str()));
      CGameState::getSingleton().setMapPack(pMapPack);
      //eGameStateToChangeTo = CGameState::GS_GAME;
    }
    else if (snapshot.getGameState() == CGameState::GS_STATISTICS) {
      CGameState::getSingleton().getStatistics().readFromXMLElement(pSnapshotElem->FirstChildElement("statistics"));
    }

    CGameState::getSingleton().changeGameState(eGameStateToChangeTo, true, false);
    
    switch (CGameState::getSingleton().getCurrentGameState()) {
    case CGameState::GS_AD:
      {
      }
      break;
    case CGameState::GS_GAME:
      { 
	CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
	assert(pScreenplay);
	pScreenplay->readFromXMLElement(pSnapshotElem->FirstChildElement("screenplay"));
      }
      break;
    case CGameState::GS_STATISTICS:
      {
	std::shared_ptr<CMapPack> pMapPack(new CMapPack(Attribute(pSnapshotElem, "map_name").c_str()));
	CGameState::getSingleton().setMapPack(pMapPack);
      }
      break;
    default:
      // not supported yet... state is not saved
      break;
    }
  }
  catch (const Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Error on loading a snapshot.");
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, e.getFullDescription());
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU, MainMenu::MMS_START);
  }
  catch (...) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Unknown error on loading a snapshot.");
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU, MainMenu::MMS_START);
  }


  // this is a very dirty hack, that prevent on android jittering!
  // appearing when no input has been done, and goming to game (from ad or game, not userinput needed)
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(1, 1);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);

  Ogre::LogManager::getSingleton().logMessage("Loading snapshot done.");
}
void CSnapshotManager::createFromFile(const Ogre::String &name) {
  CSnapshot *pSnapshot = new CSnapshot();
  pSnapshot->getXMLDocument().LoadFile(name.c_str());
  if (pSnapshot->getXMLDocument().Error()) {
    delete pSnapshot;
    return;
  }

  pSnapshot->setGameState(static_cast<CGameState::EGameStates>(pSnapshot->getXMLDocument().FirstChildElement("snapshot")->IntAttribute("game_state")));

  m_vSnapshots.push_back(pSnapshot);
}
