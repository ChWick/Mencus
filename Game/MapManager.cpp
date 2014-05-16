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

#include "MapManager.hpp"
#include "Map.hpp"

CMapManager::CMapManager(Ogre::SceneManager *pSceneManager)
  : m_pSceneManager(pSceneManager),
    m_pCurrentMap(NULL) {
}
CMapManager::~CMapManager() {
  if (m_pCurrentMap) {delete m_pCurrentMap; m_pCurrentMap = NULL;}
}
