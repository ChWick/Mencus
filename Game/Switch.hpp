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
  enum ESwitchStates {
    SS_ACTIVATED,
    SS_DEACTIVATED,
  };
  static Ogre::String getPreviewImageName(int iType);
  const SwitchType m_stSwitchType;
  ESwitchStates m_eSwitchState;
  vector<SSwitchEntry> m_vEntries;
  vector<STogglesLinkEntry> m_vLinkEntries;
  bool m_bChangeBlocks;
public:
  CSwitch(const CSpriteTransformPipeline *pTransformPipeline,
	  Ogre2dManager *pSpriteManager,
	  const Ogre::Vector2 &vPosition,
	  SwitchType stSwitchType,
	  bool bChangeBlocks,
	  ESwitchStates eSwitchState);
  ~CSwitch();

  void initialize(CMap *pMap);

  void addEntry(const SSwitchEntry &entry) {m_vEntries.push_back(entry);}
  void addEntry(const STogglesLinkEntry &entry) {m_vLinkEntries.push_back(entry);}
  void eraseEntry(int iAt) {m_vEntries.erase(m_vEntries.begin() + iAt);}
  void eraseLinkEntry(int iAt) {m_vLinkEntries.erase(m_vLinkEntries.begin() + iAt);}
  void setChangeBlocks(bool b) {m_bChangeBlocks = b;}

  bool doesChangeBlocks() const {return m_bChangeBlocks;}
  const vector<SSwitchEntry> &getEntries() const {return m_vEntries;}
  const vector<STogglesLinkEntry> &getLinkEntries() const {return m_vLinkEntries;}
  ESwitchStates getState() const {return m_eSwitchState;}
  SwitchType getType() const {return m_stSwitchType;}

  void activate(CMap *pMap);

private:
  void place(CMap *pMap, const SSwitchEntry &entry);
};

#endif
