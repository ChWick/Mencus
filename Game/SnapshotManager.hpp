#ifndef _SNAPSHOT_MANAGER_HPP_
#define _SNAPSHOT_MANAGER_HPP_

#include <OgreSingleton.h>
#include "Snapshot.hpp"
#include <vector>
#include <memory>

class CSnapshotManager :
  public Ogre::Singleton<CSnapshotManager> {

private:
  std::vector<CSnapshot*> m_vSnapshots;
  std::shared_ptr<CSnapshot> m_pSnapshotToLoad;
public:
  static CSnapshotManager& getSingleton(void);
  static CSnapshotManager* getSingletonPtr(void);

  ~CSnapshotManager();

  bool loadFromSnapshot() {
    if (!m_pSnapshotToLoad) {return false;}
    
    loadFromSnapshot(*m_pSnapshotToLoad.get());

    return true;
  }

  void setSnapshot(std::shared_ptr<CSnapshot> pSnapshot) {
    m_pSnapshotToLoad = pSnapshot;
  }

  const CSnapshot &makeSnapshot(CSnapshot *pSnapshot = NULL);
  const CSnapshot &makeBackupSnapshot();
  // this is only for testing
  void createFromFile(const Ogre::String &name);

  void loadFromSnapshot(const CSnapshot &snapshot);
  bool loadBackupSnapshot();

private:
  const CSnapshot &makeSnapshot(const Ogre::String &name, CSnapshot *pOrigin);
};

#endif
