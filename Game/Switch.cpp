#include "Switch.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "XMLHelper.hpp"
#include "Events/Event.hpp"
#include "Events/ChangeTileEvent.hpp"
#include "Events/ToggleEvent.hpp"

using namespace XMLHelper;

const Ogre::Vector2 SWITCH_SIZES[CSwitch::SWITCH_COUNT] = {
  Ogre::Vector2(0.25, 0.25),
  Ogre::Vector2(1, 1)
};
const unsigned int SWITCH_FLAGS[CSwitch::SWITCH_COUNT] = {
  CSwitch::SF_SIMPLE,
  CSwitch::SF_DEACTIVATABLE
};

CSwitch::CSwitch(CMap &map,
		 const std::string &sID,
		 CEntity *pParent,
		 const Ogre::Vector2 &vPosition,
		 SwitchType stSwitchType,
		 bool bChangeBlocks,
		 ESwitchStates eSwitchState)
  : CSprite(map,
	    sID,
	    pParent,
	    &map,
	    map.get2dManager(),
	    vPosition,
	    SWITCH_SIZES[stSwitchType]),
    m_fTimer(0),
    m_fActiveTime(0),
    m_stSwitchType(stSwitchType),
    m_eSwitchState(eSwitchState) {
  m_uiSwitchFlags = SWITCH_FLAGS[stSwitchType];
  if (bChangeBlocks) {
    m_uiSwitchFlags |= SF_CHANGE_BLOCKS;
  }
  setTexture(getSwitchTexture(stSwitchType, false));
}
CSwitch::CSwitch(CMap &map,
		 CEntity *pParent,
		 const tinyxml2::XMLElement *pElem) 
  : CSprite(map,
	    pParent,
	    &map,
	    map.get2dManager(),
	    pElem,
	    SWITCH_SIZES[EnumAttribute<ESwitchTypes>(pElem, "type")]),
    m_fTimer(RealAttribute(pElem, "timer", 0)),
    m_fActiveTime(RealAttribute(pElem, "activeTime")),
    m_stSwitchType(EnumAttribute<ESwitchTypes>(pElem, "type")),
    m_eSwitchState(EnumAttribute<ESwitchStates>(pElem, "state", SS_DEACTIVATED)),
    m_uiSwitchFlags(IntAttribute(pElem, "flags", SWITCH_FLAGS[EnumAttribute<ESwitchTypes>(pElem, "type")])) {

  // ===========================================================
  // for old map format
  if (BoolAttribute(pElem, "affectsBlocks", false)) {
    m_uiSwitchFlags |= SF_CHANGE_BLOCKS;
  }
  // ===========================================================

  using namespace tinyxml2;

  for (const XMLElement *pChange = pElem->FirstChildElement(); pChange; pChange = pChange->NextSiblingElement()) {
    // for supporting old maps
    if (strcmp(pChange->Value(), "togglesLink") == 0) {
      addEvent(new CToggleEvent(m_Map, pChange));
    }
    else if (strcmp(pChange->Value(), "changes") == 0) {
      CChangeTileEvent *pEvent = new CChangeTileEvent(m_Map, pChange);
      addEvent(pEvent);
    }
  }

  setTexture(getSwitchTexture(m_stSwitchType, m_eSwitchState != SS_DEACTIVATED));
}

CSwitch::~CSwitch() {
}
void CSwitch::update(Ogre::Real tpf) {
  CSprite::update(tpf);
  if (m_eSwitchState == SS_DEACTIVATING) {
    m_fTimer -= tpf;
    if (m_fTimer <= 0) {
      deactivate();
    }
  }
}
void CSwitch::activate() {
  if (m_eSwitchState == SS_DEACTIVATED || m_eSwitchState == SS_DEACTIVATING) {
    if (m_uiSwitchFlags & SF_TIMED) {
      m_fTimer = m_fActiveTime;
      updateState(SS_DEACTIVATING);
    }
    else {
      updateState(SS_ACTIVATED);
    }
  }
  else {
    if (m_uiSwitchFlags & SF_DEACTIVATABLE) {
      updateState(SS_DEACTIVATED);
    }
    else {
      return;
    }
  }
  
}
void CSwitch::deactivate() {
  updateState(SS_DEACTIVATED);
}
void CSwitch::updateState(ESwitchStates eNewState) {
  if ((m_eSwitchState == SS_DEACTIVATED && eNewState != SS_DEACTIVATED) 
      || (m_eSwitchState == SS_ACTIVATED && eNewState == SS_DEACTIVATED)
      || (m_eSwitchState == SS_DEACTIVATING && eNewState == SS_DEACTIVATED)) {
    m_eSwitchState = eNewState;
    if (isFlagSet(SF_CHANGE_BLOCKS)) {
      m_Map.swapBoxes();
    }
    for (auto *pEvent : m_lEvents) {
      execute(pEvent);
    }
  }
  m_eSwitchState = eNewState;
  if (m_eSwitchState == SS_DEACTIVATED) {
    setTexture(getSwitchTexture(m_stSwitchType, false));
  }
  else {
    setTexture(getSwitchTexture(m_stSwitchType, true));
  }
}
void CSwitch::execute(CEvent *pEvent) {
  if (m_eSwitchState != SS_DEACTIVATED) {
    pEvent->start();
  }
  else {
    pEvent->stop();
  }
}
Ogre::String CSwitch::getPreviewImageName(int iType) {
  return getSwitchTexture(iType, false);
}
void CSwitch::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  using namespace tinyxml2;

  CSprite::writeToXMLElement(pElem, eStyle);
  pElem->SetAttribute("type", m_stSwitchType);
  pElem->SetAttribute("flags", m_uiSwitchFlags);
  pElem->SetAttribute("activeTime", m_fActiveTime);
  if (eStyle == OS_FULL) {
    pElem->SetAttribute("state", m_eSwitchState);
    pElem->SetAttribute("timer", m_fTimer);
  }
}
