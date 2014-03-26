#include "SnapshotManager.hpp"
#include "GameState.hpp"
#include "Screenplay.hpp"
#include "FileManager.hpp"
#include "MapInfo.hpp"
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
  return makeSnapshot("backup.xml", m_vSnapshots.back());
}
const CSnapshot &CSnapshotManager::makeSnapshot(CSnapshot *pSnapshotOrig) {
  CSnapshot *pSnapshot = pSnapshotOrig;
  if (!pSnapshotOrig) {
    pSnapshot = new CSnapshot();
    m_vSnapshots.push_back(pSnapshot);
  }
  
  return makeSnapshot("snapshot.xml", pSnapshot);
}
const CSnapshot &CSnapshotManager::makeSnapshot(const Ogre::String &name, CSnapshot *pOrigin) {
  CSnapshot &snapshot(*pOrigin);

  snapshot.setGameState(CGameState::getSingleton().getCurrentGameState());

  tinyxml2::XMLDocument &doc(snapshot.getXMLDocument());
  tinyxml2::XMLElement *pElem = doc.NewElement("snapshot");
  pElem->SetAttribute("game_state", snapshot.getGameState());
  doc.InsertEndChild(pElem);

  switch (snapshot.getGameState()) {
  case CGameState::GS_GAME:
    {
      CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
      tinyxml2::XMLElement *pScreenplayElem = doc.NewElement("screenplay");
      pElem->InsertEndChild(pScreenplayElem);
      pScreenplay->writeToXMLElement(pScreenplayElem);
    }
    break;
  case CGameState::GS_STATISTICS:
    {
      pElem->SetAttribute("map_name", CGameState::getSingleton().getMapInfo()->getFileName().c_str());
      tinyxml2::XMLElement *pStatElem = doc.NewElement("statistics");
      pElem->InsertEndChild(pStatElem);
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
    //Ogre::LogManager::getSingleton().logMessage("Loading snapshot");
    if (snapshot.getGameState() == CGameState::GS_GAME) {
      // no ad when loading from savesate
      CGameState::getSingleton().setAdShown(true);
    }

    CGameState::getSingleton().changeGameState(snapshot.getGameState(), true, false);
    const tinyxml2::XMLElement *pSnapshotElem = snapshot.getXMLDocument().FirstChildElement("snapshot");
    switch (CGameState::getSingleton().getCurrentGameState()) {
    case CGameState::GS_GAME:
      { 
	CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
	assert(pScreenplay);
	pScreenplay->readFromXMLElement(pSnapshotElem->FirstChildElement("screenplay"));

	// this is a very dirty hack, that prevent on android jittering!
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(200, 200);
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);
      }
      break;
    case CGameState::GS_STATISTICS:
      {
	std::shared_ptr<CMapInfo> pMapInfo(new CMapInfo(Attribute(pSnapshotElem, "map_name").c_str(), "level_user"));
	CGameState::getSingleton().setMapInfo(pMapInfo);
      }
      break;
    default:
      // not supported yet... state is not saved
      break;
    }
    cout << CGameState::getSingleton().getCurrentGameState() << endl;
  }
  catch (const Ogre::Exception &e) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, e.getFullDescription());
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU, true, false);
  }
  catch (...) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Unknown error on loading a snapshot.");
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU, true, false);
  }
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
