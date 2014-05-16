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

#include "TutorialManager.hpp"
#include "GUITutorial.hpp"
#include "Map.hpp"
#include "Switch.hpp"
#include "Player.hpp"
#include "Enemy.hpp"

CTutorialManager::CTutorialManager(const CMap &map)
  : m_bEnabled(false),
    m_Map(map) {
  m_bInformationShown.resize(IS_COUNT, false);
}
void CTutorialManager::start() {
}
void CTutorialManager::update() {
  if (!m_bEnabled) {return;}
  if (!m_bInformationShown[IS_MOTION]) {
    CGUITutorial::getSingleton().showMoveInformationWindow();
    m_bInformationShown[IS_MOTION] = true;
    return;
  }
  const CPlayer *pPlayer = m_Map.getPlayer();
  if (!m_bInformationShown[IS_SWITCH]) {
    for (const CEntity *pSwitch : m_Map.getSwitches()) {
      if (pSwitch->getCenter().squaredDistance(pPlayer->getCenter()) < 6) {
	CGUITutorial::getSingleton().showSwitchInformationWindow();
	m_bInformationShown[IS_SWITCH] = true;
	return;
      }
    }
  }
  if (!m_bInformationShown[IS_ENEMY] || !m_bInformationShown[IS_SMALL_ENEMY]) {
    for (const CEntity *pEnemy : m_Map.getEnemies()) {
      if (pEnemy->getCenter().squaredDistance(pPlayer->getCenter()) < 25) {
	if (!m_bInformationShown[IS_ENEMY]) {
	  CGUITutorial::getSingleton().showEnemyInformationWindow();
	  m_bInformationShown[IS_ENEMY] = true;
	  return;
	}
	if (pEnemy->getType() == CEnemy::ET_GREEN_MONSTER) {
	  CGUITutorial::getSingleton().showSmallEnemyInformationWindow();
	  m_bInformationShown[IS_SMALL_ENEMY] = true;
	  return;
	}
      }
    }
  }
}
