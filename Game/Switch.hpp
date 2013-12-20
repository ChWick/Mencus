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
  const SwitchType m_stSwitchType;
  ESwitchStates m_eSwitchState;
  vector<SSwitchEntry> m_vEntries;
  bool m_bChangeBlocks;
public:
  CSwitch(const CSpriteTransformPipeline *pTransformPipeline,
	  Ogre2dManager *pSpriteManager,
	  const Ogre::Vector2 &vPosition,
	  SwitchType stSwitchType);
  ~CSwitch();

  void initialize(const CMap *pMap);

  void addEntry(const SSwitchEntry &entry) {m_vEntries.push_back(entry);}
  void setChangeBlocks(bool b) {m_bChangeBlocks;}

  bool doesChangeBlocks() const {return m_bChangeBlocks;}
  const vector<SSwitchEntry> &getEntries() const {return m_vEntries;}
  
  void activate(CMap *pMap);

private:
  void place(CMap *pMap, const SSwitchEntry &entry);
};

#endif
