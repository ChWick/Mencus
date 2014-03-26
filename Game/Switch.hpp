#ifndef SWITCH_HPP
#define SWITCH_HPP

#include "Sprite.hpp"
#include "Grid2d.hpp"

class CMap;

typedef unsigned int SwitchType;

struct SSwitchEntry {
  unsigned int uiTileType;
  unsigned int uiOldTileType;
  unsigned int uiTilePosX;
  unsigned int uiTilePosY;

  SSwitchEntry() 
    : uiTileType(1),
      uiOldTileType(1),
      uiTilePosX(0),
      uiTilePosY(0) {
  }
  Ogre::String toString() const {
    return "at (" + Ogre::StringConverter::toString(uiTilePosX) + ", "
      + Ogre::StringConverter::toString(uiTilePosY) + ") tile " 
      + Ogre::StringConverter::toString(uiTileType);
  }
};
struct STogglesLinkEntry {
  Ogre::String sLinkID;
  bool bInitialState;
};

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
  CMap *m_pMap;
  Ogre::Real m_fTimer;
  Ogre::Real m_fActiveTime;
  const SwitchType m_stSwitchType;
  ESwitchStates m_eSwitchState;
  vector<SSwitchEntry> m_vEntries;
  vector<STogglesLinkEntry> m_vLinkEntries;
  unsigned int m_uiSwitchFlags;
public:
  CSwitch(CMap *pMap,
	  const Ogre::Vector2 &vPosition,
	  SwitchType stSwitchType,
	  bool bChangeBlocks,
	  ESwitchStates eSwitchState);
  CSwitch(CMap *pMap,
	  const tinyxml2::XMLElement *pElem);
  ~CSwitch();

  void initialize();

  void update(Ogre::Real tpf);

  void addEntry(const SSwitchEntry &entry) {m_vEntries.push_back(entry);}
  void addEntry(const STogglesLinkEntry &entry) {m_vLinkEntries.push_back(entry);}
  void eraseEntry(int iAt) {m_vEntries.erase(m_vEntries.begin() + iAt);}
  void eraseLinkEntry(int iAt) {m_vLinkEntries.erase(m_vLinkEntries.begin() + iAt);}

  const vector<SSwitchEntry> &getEntries() const {return m_vEntries;}
  vector<SSwitchEntry> &getEntries() {return m_vEntries;}
  const vector<STogglesLinkEntry> &getLinkEntries() const {return m_vLinkEntries;}
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
  void place(const SSwitchEntry &entry);
};

#endif
