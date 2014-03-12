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

  const CSnapshot &makeSnapshot();
  // this is only for testing
  void createFromFile(const Ogre::String &name);

  void loadFromSnapshot();
  void loadFromSnapshot(CSnapshot &snapshot);
};

#endif
