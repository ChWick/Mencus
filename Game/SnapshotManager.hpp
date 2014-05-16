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
  //! Create a snapshot
  /**
   * \brief this function writes the current game state to snapshot and writes the content to name
   * \param name The name of the file for the snapshot
   * \param snapshot The snapshot where the data will be assigned to
   * \returns the snapshot
   */
  const CSnapshot &makeSnapshot(const Ogre::String &name, CSnapshot &snapshot);
};

#endif
