#include "SnapshotManager.hpp"
#include "GameState.hpp"
#include "Screenplay.hpp"
#include <fstream>
 
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

const CSnapshot &CSnapshotManager::makeSnapshot() {
  CSnapshot *pSnapshot = new CSnapshot();
  CSnapshot &snapshot(*pSnapshot);

  snapshot.setGameState(CGameState::getSingleton().getCurrentGameState());
  switch (snapshot.getGameState()) {
  case CGameState::GS_GAME:
    {
      CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
      tinyxml2::XMLDocument &doc(snapshot.getXMLDocument());
      tinyxml2::XMLElement *pElem = doc.NewElement("snapshot");
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
  
  Ogre::LogManager::getSingleton().logMessage("Snapshot crated");
  // output
  // ============================================================
  std::ofstream outputfile("snapshot.xml");
  tinyxml2::XMLPrinter xmlprinter;
  snapshot.getXMLDocument().Accept(&xmlprinter);

  outputfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  outputfile << xmlprinter.CStr();
  outputfile.close();

  m_vSnapshots.push_back(pSnapshot);
  
  return *m_vSnapshots.back();
}
void CSnapshotManager::loadFromSnapshot() {
  if (m_vSnapshots.size() > 0) {
    loadFromSnapshot(*m_vSnapshots.back());
  }
}
void CSnapshotManager::loadFromSnapshot(CSnapshot &snapshot) {
  Ogre::LogManager::getSingleton().logMessage("Loading snapshot");
  CGameState::getSingleton().changeGameState(snapshot.getGameState(), true, false);
  switch (snapshot.getGameState()) {
  case CGameState::GS_GAME:
    {
      CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
      pScreenplay->readFromXMLElement(snapshot.getXMLDocument().FirstChildElement("snapshot")->FirstChildElement("screenplay"));
    }
    break;
  default:
    // not supported yet... state is not saved
    break;
  }
}
