#include "Switch.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"

const Ogre::Vector2 SWITCH_SIZES[CSwitch::SWITCH_COUNT] = {
  Ogre::Vector2(0.25, 0.25),
  Ogre::Vector2(1, 1)
};
const bool SWITCH_REUSABLE[CSwitch::SWITCH_COUNT] = {
  false,
  true
};

CSwitch::CSwitch(const CSpriteTransformPipeline *pTransformPipeline,
		 Ogre2dManager *pSpriteManager,
		 const Ogre::Vector2 &vPosition,
		 SwitchType stSwitchType)
  : CSprite(pTransformPipeline, pSpriteManager, vPosition, SWITCH_SIZES[stSwitchType]),
    m_stSwitchType(stSwitchType),
    m_eSwitchState(SS_DEACTIVATED),
    m_bChangeBlocks(false) {

  setTexture(getSwitchTexture(stSwitchType, false));
}
CSwitch::~CSwitch() {
}
void CSwitch::initialize(const CMap *pMap) {
  for (auto &entry : m_vEntries) {
    entry.uiOldTileType = pMap->getTile(entry.uiTilePosX, entry.uiTilePosY)->getTileType();
  }
}
void CSwitch::activate(CMap *pMap) {
  if (m_eSwitchState == SS_DEACTIVATED) {  
    setTexture(getSwitchTexture(m_stSwitchType, true));
    m_eSwitchState = SS_ACTIVATED;
  }
  else {
    if (SWITCH_REUSABLE[m_stSwitchType]) {
      setTexture(getSwitchTexture(m_stSwitchType, false));
      m_eSwitchState = SS_DEACTIVATED;
    }
    else {
      return;
    }
  }
  for (auto &entry : m_vEntries) {
    place(pMap, entry);
  }
}
void CSwitch::place(CMap *pMap, const SSwitchEntry &entry) {
  TileType ttToSet = (m_eSwitchState == SS_ACTIVATED) ? entry.uiTileType : entry.uiOldTileType;

  delete pMap->getTile(entry.uiTilePosX, entry.uiTilePosY);
  pMap->getTile(entry.uiTilePosX, entry.uiTilePosY)
    = new CTile(pMap,
		pMap->get2dManager(),
		Ogre::Vector2(entry.uiTilePosX, entry.uiTilePosY),
		ttToSet);
}
