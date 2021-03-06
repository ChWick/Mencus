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

#include "SaveStateManager.hpp"
#include "Player.hpp"
#include "OgreException.h"
#include <time.h>
#include "Game.hpp"
#include "FileManager.hpp"

const std::string SAVE_STATE_FILE("savestate.xml");

template<> CSaveStateManager *Ogre::Singleton<CSaveStateManager>::msSingleton = 0;

CSaveStateManager *CSaveStateManager::getSingletonPtr() {
  return msSingleton;
}
CSaveStateManager &CSaveStateManager::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}


CSaveStateManager::CSaveStateManager() {
  using namespace tinyxml2;
  XMLDocument doc;

  //Ogre::String text(CGame::getSingleton().getFileAsText(SAVE_STATE_FILE));
  doc.LoadFile(CFileManager::getValidPath(SAVE_STATE_FILE).c_str());
  if (doc.Error()) {
    Ogre::LogManager::getSingleton().logMessage(SAVE_STATE_FILE + " not found.");
    return;
  }
  
  Ogre::LogManager::getSingleton().logMessage("Reading savestates");
  //Ogre::LogManager::getSingleton().logMessage(text);

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

    tm tmTime;
    tmTime.tm_sec = pSaveStateElem->IntAttribute("tm_sec");
    tmTime.tm_min = pSaveStateElem->IntAttribute("tm_min");
    tmTime.tm_hour = pSaveStateElem->IntAttribute("tm_hour");
    tmTime.tm_mday = pSaveStateElem->IntAttribute("tm_mday");
    tmTime.tm_mon = pSaveStateElem->IntAttribute("tm_mon");
    tmTime.tm_year = pSaveStateElem->IntAttribute("tm_year");

    m_vSaveStates.push_front(CSaveState(uiActID, uiSceneID, fHP, fMP, bAccessible, tmTime));
  }
  Ogre::LogManager::getSingleton().
    logMessage(Ogre::StringConverter::toString(m_vSaveStates.size()) + " savestates read.");
}
CSaveStateManager::~CSaveStateManager() {
  writeXMLFile();
}
void CSaveStateManager::write(unsigned int uiAct, unsigned int uiScene, CPlayer *pPlayer) {
  if (hasSaveState(uiAct, uiScene)) {eraseSaveState(uiAct, uiScene);}

  time_t rawtime;
  struct tm *ptm;
  time(&rawtime);
  ptm = gmtime(&rawtime);

  float fHP(-1);
  float fMP(-1);

  if (pPlayer) {
    fHP = pPlayer->getHitpoints();
    fMP = pPlayer->getManapoints();
  }

  m_vSaveStates.push_back(CSaveState(uiAct, uiScene, fHP, fMP, true, *ptm));
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
  for (auto it = m_vSaveStates.begin(); it != std::end(m_vSaveStates); it++) {
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
  if (!CGame::getSingletonPtr()) {
    return;
  }
  using namespace tinyxml2;

  XMLDocument doc;
  XMLElement *pStatesElem = doc.NewElement("save_states");
  doc.InsertEndChild(pStatesElem);

  for (const CSaveState &state : m_vSaveStates) {
    XMLElement *pStateElem = doc.NewElement("state");
    pStatesElem->InsertEndChild(pStateElem);
    pStateElem->SetAttribute("act", state.getActID());
    pStateElem->SetAttribute("scene", state.getSceneID());
    pStateElem->SetAttribute("hp", state.getPlayerHP());
    pStateElem->SetAttribute("mp", state.getPlayerMP());
    pStateElem->SetAttribute("accessible", state.isAccessible());
    pStateElem->SetAttribute("tm_sec", state.getTime().tm_sec);
    pStateElem->SetAttribute("tm_min", state.getTime().tm_min);
    pStateElem->SetAttribute("tm_hour", state.getTime().tm_hour);
    pStateElem->SetAttribute("tm_mday", state.getTime().tm_mday);
    pStateElem->SetAttribute("tm_mon", state.getTime().tm_mon);
    pStateElem->SetAttribute("tm_year", state.getTime().tm_year);
  }

  XMLPrinter xmlprinter;
  doc.Accept(&xmlprinter);

  Ogre::String header("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  Ogre::String text(xmlprinter.CStr());

  fstream stream;
  if (!CFileManager::openFile(stream, SAVE_STATE_FILE, std::ofstream::out | std::ofstream::trunc)) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Save state file could not be opened");
  }
  else {
    stream << header;
    stream << text;
    stream.close();
    //CGame::getSingleton().writeToFile(SAVE_STATE_FILE, header + text);
    Ogre::LogManager::getSingleton().logMessage("Save state file written");
    Ogre::LogManager::getSingleton().logMessage(header + text);
  }
}
