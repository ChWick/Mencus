#ifndef _SNAPSHOT_MANAGER_HPP_
#define _SNAPSHOT_MANAGER_HPP_

#include <OgreSingleton.h>
#include "Snapshot.hpp"
#include <vector>

class CSnapshotManager :
  public Ogre::Singleton<CSnapshotManager> {

private:
  std::vector<CSnapshot*> m_vSnapshots;
public:
  static CSnapshotManager& getSingleton(void);
  static CSnapshotManager* getSingletonPtr(void);

  ~CSnapshotManager();

  bool loadFromSnapshot() {
    if (m_vSnapshots.size() == 0) {
      return false;
    }
    loadFromSnapshot(*m_vSnapshots.back());
    return true;
  }

  void setSnapshot(CSnapshot *pSnapshot) {
    m_vSnapshots.push_back(pSnapshot);
  }

  const CSnapshot &makeSnapshot(CSnapshot *pSnapshot = NULL);
  // this is only for testing
  void createFromFile(const Ogre::String &name);

  void loadFromSnapshot(const CSnapshot &snapshot);
};

#endif
