#include "Switch.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

const Ogre::Vector2 SWITCH_SIZES[CSwitch::SWITCH_COUNT] = {
  Ogre::Vector2(0.25, 0.25),
  Ogre::Vector2(1, 1)
};
const unsigned int SWITCH_FLAGS[CSwitch::SWITCH_COUNT] = {
  CSwitch::SF_SIMPLE,
  CSwitch::SF_DEACTIVATABLE
};

CSwitch::CSwitch(CMap *pMap,
		 const Ogre::Vector2 &vPosition,
		 SwitchType stSwitchType,
		 bool bChangeBlocks,
		 ESwitchStates eSwitchState)
  : CSprite(pMap, pMap->get2dManager(), vPosition, SWITCH_SIZES[stSwitchType]),
    m_pMap(pMap),
    m_stSwitchType(stSwitchType),
    m_eSwitchState(eSwitchState),
    m_fTimer(0),
    m_fActiveTime(0) {
  m_uiSwitchFlags = SWITCH_FLAGS[stSwitchType];
  if (bChangeBlocks) {
    m_uiSwitchFlags |= SF_CHANGE_BLOCKS;
  }
  setTexture(getSwitchTexture(stSwitchType, false));
}
CSwitch::CSwitch(CMap *pMap,
	const tinyxml2::XMLElement *pElem) 
  : CSprite(pMap, pMap->get2dManager(), pElem, SWITCH_SIZES[EnumAttribute<ESwitchTypes>(pElem, "type")]),
    m_pMap(pMap),
    m_stSwitchType(EnumAttribute<ESwitchTypes>(pElem, "type")),
    m_eSwitchState(EnumAttribute<ESwitchStates>(pElem, "state", SS_DEACTIVATED)),
    m_uiSwitchFlags(IntAttribute(pElem, "flags", SWITCH_FLAGS[EnumAttribute<ESwitchTypes>(pElem, "type")])),
    m_fTimer(RealAttribute(pElem, "timer", 0)),
    m_fActiveTime(RealAttribute(pElem, "activeTime")) {

  // ===========================================================
  // for old map format
  if (BoolAttribute(pElem, "affectsBlocks", false)) {
    m_uiSwitchFlags |= SF_CHANGE_BLOCKS;
  }
  // ===========================================================

  using namespace tinyxml2;

  for (const XMLElement *pChange = pElem->FirstChildElement(); pChange; pChange = pChange->NextSiblingElement()) {
    if (strcmp(pChange->Value(), "togglesLink") == 0) {
      STogglesLinkEntry entry;
      entry.sLinkID = pChange->Attribute("id");
      entry.bInitialState = BoolAttribute(pChange, "initial", true);

      addEntry(entry);
    }
  }

  setTexture(getSwitchTexture(m_stSwitchType, m_eSwitchState != SS_DEACTIVATED));
}

CSwitch::~CSwitch() {
}
void CSwitch::initialize() {

  for (auto &entry : m_vLinkEntries) {
    if (!m_pMap->getLinkById(entry.sLinkID)) {
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Link with id " + entry.sLinkID + " was not found.");
      continue;
    }
    m_pMap->getLinkById(entry.sLinkID)->setActivated(entry.bInitialState);
  }

  Ogre::LogManager::getSingleton().logMessage("Created switch at (" + Ogre::StringConverter::toString(m_vPosition)
                                              + ") that affects " + Ogre::StringConverter::toString(m_vEntries.size())
                                              + " tiles" + Ogre::String((isFlagSet(SF_CHANGE_BLOCKS)) ? " and blocks." : "."));
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
      m_pMap->swapBoxes();
    }
    for (auto &entry : m_vEntries) {
      place(entry);
    }
    for (auto &entry : m_vLinkEntries) {
      m_pMap->getLinkById(entry.sLinkID)->setActivated((m_eSwitchState == SS_ACTIVATED) ? !entry.bInitialState : entry.bInitialState);
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
void CSwitch::place(const SSwitchEntry &entry) {
  TileType ttToSet = (m_eSwitchState != SS_DEACTIVATED) ? entry.uiTileType : entry.uiOldTileType;

  delete m_pMap->getTile(entry.uiTilePosX, entry.uiTilePosY);
  m_pMap->getTile(entry.uiTilePosX, entry.uiTilePosY)
    = new CTile(m_pMap,
		m_pMap->get2dManager(),
		Ogre::Vector2(entry.uiTilePosX, entry.uiTilePosY),
		ttToSet);
}
Ogre::String CSwitch::getPreviewImageName(int iType) {
  return getSwitchTexture(iType, false);
}
void CSwitch::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  using namespace tinyxml2;

  XMLDocument &doc(*pElem->GetDocument());

  CSprite::writeToXMLElement(pElem, eStyle);
  pElem->SetAttribute("type", m_stSwitchType);
  pElem->SetAttribute("flags", m_uiSwitchFlags);
  pElem->SetAttribute("activeTime", m_fActiveTime);
  if (eStyle == OS_FULL) {
    pElem->SetAttribute("state", m_eSwitchState);
    pElem->SetAttribute("timer", m_fTimer);
  }
  for (const STogglesLinkEntry &entry : getLinkEntries()) {
    XMLElement *pChange = doc.NewElement("togglesLink");
    pElem->InsertEndChild(pChange);
    pChange->SetAttribute("id", entry.sLinkID.c_str());
    pChange->SetAttribute("initial", entry.bInitialState);
  }
}
