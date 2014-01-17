#ifndef SAVE_STATE_MANAGER
#define SAVE_STATE_MANAGER

#include <tinyxml2.h>
#include <vector>

class CPlayer;

class CSaveState {
private:
  const unsigned int m_uiActID;
  const unsigned int m_uiSceneID;

  const float m_fPlayerHitpoints;
  const float m_fPlayerManapoints;

  const bool m_bAccessible;

public:
  CSaveState(unsigned int uiActID,
             unsigned int uiSceneID,
             float fHP,
             float fMP,
             bool bAccessible)
             :
               m_uiActID(uiActID),
               m_uiSceneID(uiSceneID),
               m_fPlayerHitpoints(fHP),
               m_fPlayerManapoints(fMP),
               m_bAccessible(bAccessible) {
  }
  CSaveState(const CSaveState &s)
  :
    m_uiActID(s.m_uiActID),
    m_uiSceneID(s.m_uiSceneID),
    m_fPlayerHitpoints(s.m_fPlayerHitpoints),
    m_fPlayerManapoints(s.m_fPlayerManapoints),
    m_bAccessible(s.m_bAccessible) {
  }

  unsigned int getActID() const {return m_uiActID;}
  unsigned int getSceneID() const {return m_uiSceneID;}
  float getPlayerHP() const {return m_fPlayerHitpoints;}
  float getPlayerMP() const {return m_fPlayerManapoints;}
  bool isAccessible() const {return m_bAccessible;}

};

class CSaveStateManager {
private:
  std::vector<CSaveState> m_vSaveStates;
public:
  CSaveStateManager();
  ~CSaveStateManager();

  void write(unsigned int uiAct, unsigned int uiScene, CPlayer *pPlayer);
  const CSaveState *read(unsigned int uiAct, unsigned int uiScene) const;
  bool hasSaveState(unsigned int uiAct, unsigned int uiScene) const;
  void eraseSaveState(unsigned int uiAct, unsigned int uiScene);
private:
  void writeXMLFile();
};

#endif // SAFE_STATE_MANAGER
