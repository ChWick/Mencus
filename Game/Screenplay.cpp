#include "Screenplay.hpp"
#include <OgreException.h>
#include <OgreRoot.h>
#include "Map.hpp"
#include <fstream>
#include "GUIInstructions.hpp"
#include "Video.hpp"

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
  m_pMap = new CMap(Ogre::Root::getSingleton().getSceneManager("MainSceneManager"), m_pScreenplayListener);
  m_pMap->loadMap(m_sFilename);
}
void CLevel::stop() {
  if (m_pMap) {
    delete m_pMap;
    m_pMap = 0;
  }
}
void CLevel::update(Ogre::Real tpf) {
  m_pMap->update(tpf);
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
    m_uiNextAct(0),
    m_uiNextScene(0),
    m_pCurrentScene(NULL),
    m_pOldScene(NULL) {
  CGUIInstructions::getSingleton().setScreenplayListener(this);
  parse("../level/screenplay.xml");

  setNextAct(2, 3);
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
  case CScene::ST_INSTRUCTION:
    break;
  case CScene::ST_VIDEO:
    break;
  default:
    break;
  }
  m_pCurrentScene = pScene;
  m_pOldScene = pScene;
}
void CScreenplay::parse(const Ogre::String &sFilename) {
  tinyxml2::XMLDocument doc; // namespace required for windoof...
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
        pScene = new CLevel(id, m_sLevelDir + dir + "/" + file, this);
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
      else if (type == "video") {
        XMLElement *pVideoElem = pSceneElem->FirstChildElement("video");
        CVideo *pVideo = new CVideo(id, this);

        for (XMLElement *pPartElem = pVideoElem->FirstChildElement(); pPartElem; pPartElem = pPartElem->NextSiblingElement()) {
          Ogre::String sAudioFile = pPartElem->Attribute("audio");

          CVideo::CPart *pPart = new CVideo::CPart(pVideo, sAudioFile);
          pVideo->addPart(pPart);

          for (XMLElement *pPictureElem = pPartElem->FirstChildElement(); pPictureElem; pPictureElem = pPictureElem->NextSiblingElement()) {
            Ogre::String sFile = pPictureElem->Attribute("file");
            Ogre::Real fDuration = pPictureElem->FloatAttribute("duration");
            CVideo::CPicture *pPicture = new CVideo::CPicture(m_sLevelDir + dir + "/" + sFile, fDuration, pVideo->getSpriteManager());
            pPart->addPicture(pPicture);
            for (XMLElement *pChildElem = pPictureElem->FirstChildElement(); pChildElem; pChildElem = pChildElem->NextSiblingElement()) {
              if (std::string(pChildElem->Value()) == "effect") {
                Ogre::String type(pChildElem->Attribute("type"));
                if (type == "scale") {
                  Ogre::Vector2 vCenter(Ogre::StringConverter::parseVector2(pChildElem->Attribute("center")));
                  Ogre::Vector2 vStartScale(Ogre::StringConverter::parseVector2(pChildElem->Attribute("startScale")));
                  Ogre::Vector2 vEndScale(Ogre::StringConverter::parseVector2(pChildElem->Attribute("endScale")));
                  CVideo::CPicture::CEffectScale::EScaleType eScaleType = CVideo::CPicture::CEffectScale::ST_LINEAR;
                  if (pChildElem->Attribute("scaling")) {
                    Ogre::String sScaling = pChildElem->Attribute("scaling");
                    if (sScaling == "linear") {
                      eScaleType = CVideo::CPicture::CEffectScale::ST_LINEAR;
                    }
                    else if (sScaling == "quadratic") {
                      eScaleType = CVideo::CPicture::CEffectScale::ST_QUADRATIC;
                    }
                    else {
                      throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS, sScaling + " is unknown as a scaling type!", __FILE__);
                    }
                  }

                  pPicture->addEffect(new CVideo::CPicture::CEffectScale(vCenter, vStartScale, vEndScale, eScaleType));
                }
              }
            }
          }
        }

        pScene = pVideo;
      }
      pAct->addScene(pScene);
    }

    m_mapActs[id] = pAct;
  }
}
void CScreenplay::update(Ogre::Real tpf) {
  if (tpf > 0.2) {
    // Probably loading causes this fps, next time step update
    return;
  }
  if (m_pCurrentScene) {
    m_pCurrentScene->update(tpf);
  }


  if (m_uiCurrentAct != m_uiNextAct || m_uiCurrentScene != m_uiNextScene) {
    loadAct(m_uiNextAct, m_uiNextScene);
  }
}
void CScreenplay::playerExitsMap() {
  m_uiNextScene++;
}
void CScreenplay::keyForContinueInstructionsPressed() {
  m_uiNextScene++;
}
void CScreenplay::videoFinished() {
  m_uiNextScene++;
}
