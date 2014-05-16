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

#include "Switch.hpp"
#include "Util.hpp"
#include "Map.hpp"
#include "Tile.hpp"
#include "XMLHelper.hpp"
#include "Events/Event.hpp"
#include "Events/EventEmitter.hpp"
#include "Events/ChangeTileEvent.hpp"
#include "Events/ToggleEvent.hpp"
#include "DebugText.hpp"

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
    m_eSwitchState(eSwitchState),
    m_pLeftTimeText(NULL) {
  setType(stSwitchType);
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
    m_eSwitchState(EnumAttribute<ESwitchStates>(pElem, "state", SS_DEACTIVATED)),
    m_uiSwitchFlags(IntAttribute(pElem, "flags", SWITCH_FLAGS[EnumAttribute<ESwitchTypes>(pElem, "type")])),
  m_pLeftTimeText(NULL) {

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

  setTexture(getSwitchTexture(m_uiType, m_eSwitchState != SS_DEACTIVATED));

  updateState(m_eSwitchState);
}

CSwitch::~CSwitch() {
  if (m_pLeftTimeText) {
    delete m_pLeftTimeText;
  }
}
void CSwitch::update(Ogre::Real tpf) {
  CSprite::update(tpf);
  if (m_eSwitchState == SS_DEACTIVATING) {
    assert(m_pLeftTimeText);
    int iRoundedTime = static_cast<int>(ceil(m_fTimer));
    m_pLeftTimeText->setText(Ogre::StringConverter::toString(iRoundedTime));
    m_pLeftTimeText->setPosition(m_Map.mapToRelativeScreenPos(getCenter()));
    m_pLeftTimeText->scale(((m_fTimer - floor(m_fTimer)) * 2 + 1));
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
      if (pEvent->getEmitter()->getType() == EventEmitter::EMIT_ON_USER) {
	execute(pEvent);
      }
    }
  }
  m_eSwitchState = eNewState;
  if (m_eSwitchState == SS_DEACTIVATED) {
    setTexture(getSwitchTexture(m_uiType, false));
  }
  else {
    setTexture(getSwitchTexture(m_uiType, true));
  }

  if (m_eSwitchState == SS_DEACTIVATING) {
    if (!m_pLeftTimeText) {
      m_pLeftTimeText = new CDebugText();
      m_pLeftTimeText->getTextArea()->setAlignment(Ogre::TextAreaOverlayElement::Center);
    }
  }
  else {
    if (m_pLeftTimeText) {
      delete m_pLeftTimeText;
      m_pLeftTimeText = NULL;
    }
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
  pElem->SetAttribute("flags", m_uiSwitchFlags);
  pElem->SetAttribute("activeTime", m_fActiveTime);
  if (eStyle == OS_FULL) {
    pElem->SetAttribute("state", m_eSwitchState);
    pElem->SetAttribute("timer", m_fTimer);
  }
}
