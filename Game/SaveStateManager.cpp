#include "SaveStateManager.hpp"
#include "OgreException.h"

CSaveStateManager::CSaveStateManager() {
  using namespace tinyxml2;

  XMLDocument doc;
  if (doc.LoadFile(sFilename.c_str())) {
    throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, sFilename + " not found.", __FILE__);
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
    bool bAccessable = pSaveStateElem->BoolAttribute("accessable");

    m_vSaveStates.pushBack(CSaveState(uiActID, uiSceneID, fHP, fMP, bAccessable));
  }
}
void write(unsigned int uiAct, unsigned int uiScene, CPlayer *pPlayer) {
}
const CSaveState *read(unsigned int uiAct, unsigned int uiScene) const {
}

  void writeXMLFile();
