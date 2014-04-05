#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "Sprite.hpp"
#include "Grid2d.hpp"

class CMap;

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
  const SwitchType m_stSwitchType;
  ESwitchStates m_eSwitchState;
  unsigned int m_uiSwitchFlags;
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
  SwitchType getType() const {return m_stSwitchType;}

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
