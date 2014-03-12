#include "SnapshotManager.hpp"
#include "GameState.hpp"
#include "Screenplay.hpp"
 
template<> CSnapshotManager* Ogre::Singleton<CSnapshotManager>::msSingleton = 0;

CSnapshotManager* CSnapshotManager::getSingletonPtr(void)
{
  return msSingleton;
}
CSnapshotManager& CSnapshotManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}


const CSnapshot &CSnapshotManager::makeSnapshot() {
  CSnapshot snapshot;
  m_vSnapshots.push_back(snapshot);

  snapshot.setGameState(CGameState::getSingleton().getCurrentGameState());
  switch (snapshot.getGameState()) {
  case CGameState::GS_GAME:
    CScreenplay *pScreenplay = CGameState::getSingleton().getScreenplay();
    tinyxml2::XMLDocument &doc(snapshot.getXMLDocument());
    tinyxml2::XMLElement *pElem = doc.NewElement("snapshot");
    doc.InsertEndChild(pElem);
    tinyxml2::XMLElement *pScreenplay = doc.NewElement("screenplay");
    pElem->InsertEndChild(pScreenplay);
    pScreenplay->writeToXMLElement(pScreenplay);
    break;
  default:
    // not supported yet... state is not saved
    break;
  }
  
  
  return m_vSnapshots.back();
}
void CSnapshotManager::loadFromSnapshot() {
  if (m_vSnapshots.size() > 0) {
    loadFromSnapshot(m_vSnapshots.back());
  }
}
void CSnapshotManager::loadFromSnapshot() {

}
