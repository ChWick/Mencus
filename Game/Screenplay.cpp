#include "Screenplay.hpp"
#include <OgreException.h>
#include <OgreRoot.h>
#include "Map.hpp"
#include <fstream>
#include "GUIInstructions.hpp"

using namespace tinyxml2;

void CInstructions::start() {
  CGUIInstructions::getSingleton().setText(m_sText);
  CGUIInstructions::getSingleton().show();
}
void CInstructions::stop() {
  CGUIInstructions::getSingleton().hide();
}

CLevel::~CLevel() {
  if (m_pMap) {
    delete m_pMap;
    m_pMap = NULL;
  }
}
void CLevel::start() {
  m_pMap = new CMap(Ogre::Root::getSingleton().getSceneManager("MainSceneManager"));
  m_pMap->loadMap(m_sFilename);
}
void CLevel::stop() {
  if (m_pMap) {
    delete m_pMap;
    m_pMap = 0;
  }
}


CAct::~CAct() {
  for (auto &p : m_mapScenes) {
    delete p.second;
  }
}

void CAct::start() {
}

void CAct::stop() {
}

CScreenplay::CScreenplay()
  : m_sLevelDir("../level/"),
    m_uiCurrentAct(0),
    m_uiCurrentScene(0),
    m_pOldScene(NULL) {
  parse("../level/screenplay.xml");

  loadAct(1, 1);
}
CScreenplay::~CScreenplay() {
  for (auto &p : m_mapActs) {
    delete p.second;
  }
}
void CScreenplay::loadAct(unsigned int uiActId, unsigned int uiSceneId) {
  if (m_pOldScene) {m_pOldScene->stop();}

  m_uiCurrentAct = uiActId;
  m_uiCurrentScene = uiSceneId;

  CAct *pAct = m_mapActs[uiActId];
  if (!pAct) {throw Ogre::Exception(0, Ogre::StringConverter::toString(uiActId) + " is no valid act id!", __FILE__);}
  CScene *pScene = pAct->getScene(uiSceneId);
  if (!pScene) {throw Ogre::Exception(0, Ogre::StringConverter::toString(uiSceneId) + " is no valid scene id in act " + Ogre::StringConverter::toString(uiActId) + "!", __FILE__);}

  pScene->start();
  switch (pScene->getType()) {
  case CScene::ST_LEVEL:
    break;
  }

  m_pOldScene = pScene;
}
void CScreenplay::parse(const Ogre::String &sFilename) {
  XMLDocument doc;
  if (doc.LoadFile(sFilename.c_str())) {
    throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, sFilename + " not found.", __FILE__);
  }

  XMLElement *pRoot = doc.FirstChildElement("screenplay");

  for (XMLElement *pActElem = pRoot->FirstChildElement(); pActElem; pActElem = pActElem->NextSiblingElement()) {

    unsigned int id = pActElem->IntAttribute("id");
    Ogre::String dir = pActElem->Attribute("dir");
    CAct *pAct = new CAct(id, dir);

    for (XMLElement *pSceneElem = pActElem->FirstChildElement(); pSceneElem; pSceneElem = pSceneElem->NextSiblingElement()) {

      unsigned int id = pSceneElem->IntAttribute("id");
      Ogre::String type = pSceneElem->Attribute("type");
      CScene *pScene(NULL);

      if (type == "level") {
        XMLElement *pLevelElem = pSceneElem->FirstChildElement("level");
        Ogre::String file = pLevelElem->Attribute("file");
        pScene = new CLevel(id, m_sLevelDir + dir + "/" + file);
      }
      else if (type == "instructions") {
        XMLElement *pInstructionsElem = pSceneElem->FirstChildElement("instructions");
        Ogre::String sFile = pInstructionsElem->Attribute("file");
        std::ifstream file(m_sLevelDir + dir + "/" + sFile);
        if (!file) {throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, "'" + m_sLevelDir + dir + "/" + sFile + "' was not found.", __FILE__);}

        file.seekg(0,std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0,std::ios::beg);
        std::vector<CEGUI::utf8> buffer(length);
        file.read((char*)(&buffer[0]), length);

        pScene = new CInstructions(id, &buffer[0]);
      }
      pAct->addScene(pScene);
    }

    m_mapActs[id] = pAct;
  }
}
