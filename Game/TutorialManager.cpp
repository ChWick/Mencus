#include "TutorialManager.hpp"
#include "GUITutorial.hpp"
#include "Map.hpp"
#include "Switch.hpp"
#include "Player.hpp"
#include "Enemy.hpp"

CTutorialManager::CTutorialManager(const CMap &map)
  : m_Map(map),
    m_bEnabled(false) {
  m_bInformationShown.resize(IS_COUNT, false);
}
void CTutorialManager::start() {
}
void CTutorialManager::update() {
  if (!m_bEnabled) {return;}
  if (!m_bInformationShown[IS_MOTION]) {
    CGUITutorial::getSingleton().showMoveInformationWindow();
    m_bInformationShown[IS_MOTION] = true;
  }
  const CPlayer *pPlayer = m_Map.getPlayer();
  if (!m_bInformationShown[IS_SWITCH]) {
    for (const CSwitch *pSwitch : m_Map.getSwitches()) {
      if (pSwitch->getCenter().squaredDistance(pPlayer->getCenter()) < 6) {
	CGUITutorial::getSingleton().showSwitchInformationWindow();
	m_bInformationShown[IS_SWITCH] = true;
	return;
      }
    }
  }
  if (!m_bInformationShown[IS_ENEMY] || !m_bInformationShown[IS_SMALL_ENEMY]) {
    for (const CEnemy *pEnemy : m_Map.getEnemies()) {
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
