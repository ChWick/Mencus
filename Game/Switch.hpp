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

#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "Sprite.hpp"
#include "Grid2d.hpp"

class CMap;
class CDebugText;

typedef unsigned int SwitchType;

class CSwitch : public CSprite {
public:
  enum ESwitchTypes : SwitchType {
    SWITCH_1,
    SWITCH_2,
    SWITCH_COUNT
  };
  enum ESwitchFlags {
    SF_SIMPLE            = 0,  //!< simple switch that can only be used once, and that will keep its state
    SF_DEACTIVATABLE     = 1,  //!< can an activated switch be deactivated
    SF_TIMED             = 2,  //!< will this switch go back to the deactivated state after m_fActiveTime
    SF_CHANGE_BLOCKS     = 4,  //!< will this switch change blocks
  };
  enum ESwitchStates {
    SS_ACTIVATED,		//!< Switch is activated 
    SS_DEACTIVATED,		//!< Switch is deactivated
    SS_DEACTIVATING		//!< Switch is going to be deactivated when m_fTimer reaches 0
  };
  static Ogre::String getPreviewImageName(int iType);
private:
  Ogre::Real m_fTimer;
  Ogre::Real m_fActiveTime;
  ESwitchStates m_eSwitchState;
  unsigned int m_uiSwitchFlags;
  CDebugText *m_pLeftTimeText;
public:
  CSwitch(CMap &map,
	  const std::string &sID,
	  CEntity *pParent,
	  const Ogre::Vector2 &vPosition,
	  SwitchType stSwitchType,
	  bool bChangeBlocks,
	  ESwitchStates eSwitchState);
  CSwitch(CMap &map,
	  CEntity *pParent,
	  const tinyxml2::XMLElement *pElem);
  ~CSwitch();

  void update(Ogre::Real tpf);

  ESwitchStates getState() const {return m_eSwitchState;}

  Ogre::Real &getActivationTime() {return m_fActiveTime;}
  Ogre::Real getActivationTime() const {return m_fActiveTime;}

  void setFlag(unsigned int uiFlag, bool bAdd = true) {if (bAdd) {addFlag(uiFlag);} else {removeFlag(uiFlag);}}
  void addFlag(unsigned int uiFlag) {m_uiSwitchFlags |= uiFlag;}
  void removeFlag(unsigned int uiFlag) {m_uiSwitchFlags -= m_uiSwitchFlags & uiFlag;}
  bool isFlagSet(unsigned int uiFlag) const {return m_uiSwitchFlags & uiFlag;}

  void activate();
  void deactivate();

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;
private:
  void updateState(ESwitchStates eNewState);
  void execute(CEvent *pEvent);
};

#endif
