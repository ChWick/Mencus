#include "SaveStateManager.hpp"
#include "Player.hpp"
#include "OgreException.h"

CSaveStateManager::CSaveStateManager() {
  using namespace tinyxml2;
  std::string sFilename("savestate.xml");
  XMLDocument doc;
  if (doc.LoadFile(sFilename.c_str())) {
    Ogre::LogManager::getSingleton().logMessage(sFilename + " not found.");
    return;
  }


  XMLElement *pRoot = doc.FirstChildElement("save_states");
  if (!pRoot) {
    return; // no elements in the save state
  }

  for (XMLElement *pSaveStateElem = pRoot->FirstChildElement(); pSaveStateElem; pSaveStateElem = pSaveStateElem->NextSiblingElement()) {
    unsigned int uiActID = pSaveStateElem->IntAttribute("act");
    unsigned int uiSceneID = pSaveStateElem->IntAttribute("scene");
    float fHP = pSaveStateElem->FloatAttribute("hp");
    float fMP = pSaveStateElem->FloatAttribute("mp");
    bool bAccessible = pSaveStateElem->BoolAttribute("accessible");

    m_vSaveStates.push_back(CSaveState(uiActID, uiSceneID, fHP, fMP, bAccessible));
  }
}
CSaveStateManager::~CSaveStateManager() {
  writeXMLFile();
}
void CSaveStateManager::write(unsigned int uiAct, unsigned int uiScene, CPlayer *pPlayer) {
  if (hasSaveState(uiAct, uiScene)) {eraseSaveState(uiAct, uiScene);}

  m_vSaveStates.push_back(CSaveState(uiAct, uiScene, pPlayer->getHitpoints(), pPlayer->getManapoints(), true));
  writeXMLFile();
}
bool CSaveStateManager::hasSaveState(unsigned int uiAct, unsigned int uiScene) const {
  for (const CSaveState &state : m_vSaveStates) {
    if (state.getActID() == uiAct && state.getSceneID() == uiScene) {
      return true;
    }
  }
  return false;
}
void CSaveStateManager::eraseSaveState(unsigned int uiAct, unsigned int uiScene) {
  for (auto it = std::begin(m_vSaveStates); it != std::end(m_vSaveStates); it++) {
    if (it->getActID() == uiAct && it->getSceneID() == uiScene) {
      m_vSaveStates.erase(it);
      return;
    }
  }
}
const CSaveState *CSaveStateManager::read(unsigned int uiAct, unsigned int uiScene) const {
  for (const CSaveState &state : m_vSaveStates) {
    if (state.getActID() == uiAct && state.getSceneID() == uiScene) {
      return &state;
    }
  }
  return NULL;
}
void CSaveStateManager::writeXMLFile() {
  using namespace tinyxml2;

  XMLDocument doc;
  XMLElement *pStatesElem = doc.NewElement("save_states");
  doc.InsertEndChild(pStatesElem);

  for (const CSaveState &state : m_vSaveStates) {
    XMLElement *pStateElem = doc.NewElement("state");
    pStateElem->InsertEndChild(pStateElem);
    pStateElem->SetAttribute("act", state.getActID());
    pStateElem->SetAttribute("scene", state.getSceneID());
    pStateElem->SetAttribute("hp", state.getPlayerHP());
    pStateElem->SetAttribute("mp", state.getPlayerMP());
    pStateElem->SetAttribute("accessible", state.isAccessible());
  }
}
