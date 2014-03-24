#include "SnapshotManager.hpp"
#include "GameState.hpp"
#include "Screenplay.hpp"
#include "FileManager.hpp"
#include <fstream>

using namespace std;
 
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
  switch (snapshot.getGameState()) {
  case CGameState::GS_GAME:
    {
      CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
      tinyxml2::XMLDocument &doc(snapshot.getXMLDocument());
      tinyxml2::XMLElement *pElem = doc.NewElement("snapshot");
      pElem->SetAttribute("game_state", snapshot.getGameState());
      doc.InsertEndChild(pElem);
      tinyxml2::XMLElement *pScreenplayElem = doc.NewElement("screenplay");
      pElem->InsertEndChild(pScreenplayElem);
      pScreenplay->writeToXMLElement(pScreenplayElem);
    }
    break;
  default:
    // not supported yet... state is not saved
    break;
  }
  
  //Ogre::LogManager::getSingleton().logMessage("Snapshot crated");
  // output
  // ============================================================
  std::fstream outputfile;
  if (CFileManager::openFile(outputfile, name, CFileManager::SL_INTERNAL)) {
    tinyxml2::XMLPrinter xmlprinter;
    snapshot.getXMLDocument().Accept(&xmlprinter);
    
    outputfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    outputfile << xmlprinter.CStr();
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
  cout << " load" << endl;
  //Ogre::LogManager::getSingleton().logMessage("Loading snapshot");
  CGameState::getSingleton().setAdShown(true); // no ad when loading from savesate
  CGameState::getSingleton().changeGameState(snapshot.getGameState(), true, false);
  switch (CGameState::getSingleton().getCurrentGameState()) {
  case CGameState::GS_GAME:
    {
      CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
      assert(pScreenplay);
      pScreenplay->readFromXMLElement(snapshot.getXMLDocument().FirstChildElement("snapshot")->FirstChildElement("screenplay"));
    }
    break;
  default:
    // not supported yet... state is not saved
    break;
  }
  cout << CGameState::getSingleton().getCurrentGameState() << endl;
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
