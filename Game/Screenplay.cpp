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

#include "Screenplay.hpp"
#include <OgreException.h>
#include <OgreRoot.h>
#include "Map.hpp"
#include <fstream>
#include "MainMenu.hpp"
#include "GUIInstructions.hpp"
#include "Video.hpp"
#include "SaveStateManager.hpp"
#include "SaveStateManager.hpp"
#include "GameState.hpp"
#include "XMLHelper.hpp"
#include "MapInfo.hpp"
#include "GUIStatistics.hpp"
#include "Game.hpp"
#include "MessageHandler.hpp"
#include "Message.hpp"

using namespace tinyxml2;
using namespace XMLHelper;

const Ogre::Real SCREENPLAY_FADE_DURATION = 0.5;

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
  m_pMap->loadMap(m_pMapPack);
}
void CLevel::stop() {
  
}
void CLevel::init() {
  if (!m_pMap) {
    m_pMap = new CMap(Ogre::Root::getSingleton().getSceneManager("MainSceneManager"),
		      m_pScreenplayListener,
		      CGameState::getSingleton().getStatistics(),
		      m_pMapPack);
  }
}
void CLevel::exit() {
  if (m_pMap) {
    delete m_pMap;
    m_pMap = 0;
  }
}
void CLevel::update(Ogre::Real tpf) {
  m_pMap->update(tpf);
}
void CLevel::render(Ogre::Real tpf) {
  m_pMap->render(tpf);
}
void CLevel::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  m_pMap->writeToXMLElement(pElem, eStyle);
}
void CLevel::readFromXMLElement(const tinyxml2::XMLElement *pElem) {
  m_pMap->readFromXMLElement(pElem);
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
  : m_eScreenplayType(SCREENPLAY_NONE),
    m_sResourceGroup("mencus_campaign"),
    m_uiCurrentAct(0),
    m_uiCurrentScene(0),
    m_uiNextAct(0),
    m_uiNextScene(0),
    m_pCurrentScene(NULL),
    m_pOldScene(NULL),
    m_Fader(this),
    m_bPaused(false) {
  CInputListenerManager::getSingleton().addInputListener(this);
  CGUIInstructions::getSingleton().setScreenplayListener(this);
  
  m_uiNextAct = 1;
  m_uiNextScene = 1;

  /*std::shared_ptr<const CMapInfo> pMapInfo = CGameState::getSingleton().getMapInfo();
  if (pMapInfo) {
    
  }
  else {
    parse("screenplay.xml", m_sResourceGroup);

    m_uiNextAct = 1;
    m_uiNextScene = 1;
    const CSaveState *pSaveState = CGameState::getSingleton().getSaveState();
    if (pSaveState) {
      m_uiNextAct = pSaveState->getActID();
      m_uiNextScene = pSaveState->getSceneID();
    }
  }*/
}
CScreenplay::~CScreenplay() {
  m_Fader.setVisible(false);
  clear();
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CScreenplay::loadSingleMap(std::shared_ptr<const CMapPack> pMapPack) {
  assert(pMapPack);
  m_eScreenplayType = SCREENPLAY_SINGLE_MAP;
  m_pMapPack = pMapPack;

  clear();
  // create dummy act and dummy scene
  CAct *pAct = new CAct(*this, 1, "");
  CScene *pScene = new CLevel(*pAct, 1, pMapPack, this);
  pAct->addScene(pScene);
  m_mapActs[1] = pAct;

  m_Fader.setVisible(true);
  m_Fader.startFadeOut(0);
  m_uiNextAct = 1;
  m_uiNextScene = 1;
}
void CScreenplay::clear() {
  Ogre::LogManager::getSingleton().logMessage("Clearing screenplay");
  for (auto &p : m_mapActs) {
    delete p.second;
  }
  //Ogre::ResourceGroupManager::getSingleton().unloadResourceGroup(m_sResourceGroup);
}
void CScreenplay::loadAct(unsigned int uiActId, unsigned int uiSceneId) {
  Ogre::LogManager::getSingleton().logMessage("Switching to act " + Ogre::StringConverter::toString(uiActId) + " at scene " + Ogre::StringConverter::toString(uiSceneId));

  if (m_mapActs.find(uiActId) == m_mapActs.end()) {
    CLevel *pLevel = dynamic_cast<CLevel*>(m_pOldScene);
    if (pLevel) {
      SStatistics &stats = CGameState::getSingleton().getStatistics();
      stats.eMissionState = MissionState::MS_ACCOMPLISHED;
      CGameState::getSingleton().changeGameState(CGameState::GS_STATISTICS, pLevel->getMapPack());
      CMessageHandler::getSingleton()
	.addMessage(CMessage(CMessage::MT_AT_LEVEL_END)
		    .setStatistics(&stats));
    }
    else {
      CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU);
    }
    return;
  }

  if (m_pOldScene) {m_pOldScene->stop(); m_pOldScene->exit();}


  m_uiCurrentAct = uiActId;
  m_uiCurrentScene = uiSceneId;

  CAct *pAct = m_mapActs[uiActId];
  if (!pAct) {throw Ogre::Exception(0, Ogre::StringConverter::toString(uiActId) + " is no valid act id!", __FILE__);}
  CScene *pScene = pAct->getScene(uiSceneId);
  if (!pScene) {throw Ogre::Exception(0, Ogre::StringConverter::toString(uiSceneId) + " is no valid scene id in act " + Ogre::StringConverter::toString(uiActId) + "!", __FILE__);}

  CPlayer *pPlayer(NULL);

  pScene->init();
  pScene->start();
  switch (pScene->getType()) {
  case CScene::ST_LEVEL:
    pPlayer = dynamic_cast<CLevel*>(pScene)->getMap()->getPlayer();
    break;
  case CScene::ST_INSTRUCTION:
    break;
  case CScene::ST_VIDEO:
    break;
  default:
    break;
  }

  // create save state
  CSaveStateManager::getSingleton().write(m_uiCurrentAct, m_uiCurrentScene, pPlayer);

  m_pCurrentScene = pScene;
  m_pOldScene = pScene;
}
void CScreenplay::parse(const Ogre::String &sFilename, const Ogre::String &sResourceGroup) {
  clear();
  CGame::getSingleton().showLoadingBar();
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(m_sResourceGroup);
  CGame::getSingleton().hideLoadingBar();

  tinyxml2::XMLDocument doc; // namespace required for windoof...
  Ogre::DataStreamPtr pStream = Ogre::ResourceGroupManager::getSingleton().openResource(sFilename, sResourceGroup);

  doc.Parse( pStream->getAsString().c_str() );
  pStream->close();
  pStream.setNull();
  if (doc.Error()) {
    throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, sFilename + " not found.", __FILE__);
  }

  XMLElement *pRoot = doc.FirstChildElement("screenplay");

  for (XMLElement *pActElem = pRoot->FirstChildElement(); pActElem; pActElem = pActElem->NextSiblingElement()) {

    unsigned int id = pActElem->IntAttribute("id");
    Ogre::String dir = pActElem->Attribute("dir");
    CAct *pAct = new CAct(*this, id, dir);

    for (XMLElement *pSceneElem = pActElem->FirstChildElement(); pSceneElem; pSceneElem = pSceneElem->NextSiblingElement()) {

      unsigned int id = pSceneElem->IntAttribute("id");
      Ogre::String type = pSceneElem->Attribute("type");
      CScene *pScene(NULL);

      if (type == "level") {
        XMLElement *pLevelElem = pSceneElem->FirstChildElement("level");
        Ogre::String file = pLevelElem->Attribute("file");
        pScene = new CLevel(*pAct, id, shared_ptr<CMapPack>(new CMapPack(std::shared_ptr<CMapInfo>(new CMapInfo(dir + "/" + file, sResourceGroup)))), this);
      }
      else if (type == "instructions") {
        XMLElement *pInstructionsElem = pSceneElem->FirstChildElement("instructions");
        Ogre::String sFile = pInstructionsElem->Attribute("file");
	Ogre::DataStreamPtr pFile = Ogre::ResourceGroupManager::getSingleton().openResource(dir + "/" + sFile, sResourceGroup);
        
        /*file.seekg(0,std::ios::end);
        std::streampos length = file.tellg();
        file.seekg(0,std::ios::beg);
        std::vector<CEGUI::utf8> buffer(length);
        file.read((char*)(&buffer[0]), length);*/

        //pScene = new CInstructions(id, &buffer[0]);
	pScene = new CInstructions(*pAct, id, pFile->getAsString().c_str());
      }
      else if (type == "video") {
        /*XMLElement *pVideoElem = pSceneElem->FirstChildElement("video");
        CVideo *pVideo = new CVideo(*pAct, id, this);

        for (XMLElement *pPartElem = pVideoElem->FirstChildElement(); pPartElem; pPartElem = pPartElem->NextSiblingElement()) {
          Ogre::String sAudioFile = pPartElem->Attribute("audio");

          CVideo::CPart *pPart = new CVideo::CPart(pVideo, sAudioFile);
          pVideo->addPart(pPart);

          for (XMLElement *pPictureElem = pPartElem->FirstChildElement(); pPictureElem; pPictureElem = pPictureElem->NextSiblingElement()) {
            Ogre::String sFile = pPictureElem->Attribute("file");
            Ogre::Real fDuration = pPictureElem->FloatAttribute("duration");
	    Ogre::Vector2 texPos(0, 1);
	    Ogre::Vector2 texSize(1, 0);
            CVideo::CPicture *pPicture = new CVideo::CPicture(dir + "/" + sFile, fDuration, pVideo->getSpriteManager());
	    Ogre::String texStyle(Attribute(pPictureElem, "texstyle", "default"));
	    if (texStyle == "widescreen") {
	      texPos = Ogre::Vector2(0, 0.667);
	    }
	    pPicture->getSprite().setTextureCoords(texPos, texSize);
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

        pScene = pVideo;*/
      }
      pAct->addScene(pScene);
    }

    m_mapActs[id] = pAct;
  }
}
void CScreenplay::update(Ogre::Real tpf) {
  if (m_bPaused) {
    return;
  }

  if (m_Fader.isFading() && tpf > 0.1) {
    m_Fader.fade(0);
    // Probably loading causes this fps, next time step update
    return;
  }
  
  if (m_lLastFrames.size() != TPF_AVERAGE_COUNT) {
    m_lLastFrames.clear();
    for (int i = 0; i < TPF_AVERAGE_COUNT; i++) {
      m_lLastFrames.push_back(tpf);
    }
    m_fAveragedTpf = tpf;
  }
  else {
    m_lLastFrames.push_back(tpf);
    m_fAveragedTpf = (m_fAveragedTpf * TPF_AVERAGE_COUNT - m_lLastFrames.front() + tpf) / TPF_AVERAGE_COUNT;
    m_lLastFrames.pop_front();
  }

  if (m_Fader.isFading()) {
    m_Fader.fade(m_fAveragedTpf);
  }
  
  if (m_pCurrentScene) {
    if (m_fAveragedTpf > 0.05) {
      float fUpdated = 0.05;
      while (1) {
	m_pCurrentScene->update(0.05);
	fUpdated += 0.05;
	if (fUpdated > m_fAveragedTpf) {
	  m_pCurrentScene->update(fUpdated - m_fAveragedTpf);
	  break;
	}
      }
    } else {
      m_pCurrentScene->update(m_fAveragedTpf);
    }
    m_pCurrentScene->render(m_fAveragedTpf);
  }

}
bool CScreenplay::keyPressed( const OIS::KeyEvent &arg ) {
  if (arg.key == OIS::KC_ESCAPE) {
    CMainMenu::getSingleton().show();
    CMainMenu::getSingleton().changeState(MainMenu::MMS_GAME_ESCAPE);
  }
  return true;
}
bool CScreenplay::keyReleased( const OIS::KeyEvent &arg ) {
  return true;
}
void CScreenplay::playerExitsMap() {
  toNextScene();
}
void CScreenplay::keyForContinueInstructionsPressed() {
  toNextScene();
}
void CScreenplay::videoFinished() {
  toNextScene();
}
void CScreenplay::toNextScene() {
  if (m_Fader.isFading()) {return;}

  m_uiNextScene++;
  if (m_uiNextScene > m_mapActs[m_uiCurrentAct]->getSceneCount()) {
    Ogre::LogManager::getSingleton().logMessage("Switching to new act: " + Ogre::StringConverter::toString(m_uiNextAct));
    m_uiNextScene = 1;
    m_uiNextAct++;
  }
  m_Fader.startFadeOut(SCREENPLAY_FADE_DURATION);
  pause(PAUSE_ALL ^ PAUSE_SCREENPLAY ^ PAUSE_MAP_RENDER ^ PAUSE_MAP_UPDATE);
}
void CScreenplay::fadeInCallback() {
  unpause(PAUSE_ALL);
}
void CScreenplay::fadeOutCallback() {
  if (m_uiCurrentAct != m_uiNextAct || m_uiCurrentScene != m_uiNextScene) {
    m_Fader.setVisible(false);
    loadAct(m_uiNextAct, m_uiNextScene);
    m_Fader.setVisible(true);
  }
  pause(PAUSE_ALL ^ PAUSE_SCREENPLAY ^ PAUSE_MAP_RENDER ^ PAUSE_MAP_UPDATE);
  m_Fader.startFadeIn(SCREENPLAY_FADE_DURATION);
}
void CScreenplay::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  //pElem->SetAttribute("resourceGroup", m_sResourceGroup.c_str());
  pElem->SetAttribute("type", m_eScreenplayType);
  pElem->SetAttribute("currentAct", m_uiCurrentAct);
  pElem->SetAttribute("currentScene", m_uiCurrentScene);
  pElem->SetAttribute("nextAct", m_uiNextAct);
  pElem->SetAttribute("nextScene", m_uiNextScene);
 
  if (m_eScreenplayType == SCREENPLAY_SINGLE_MAP) {
    pElem->SetAttribute("map_filename", m_pMapPack->getMapInfo()->getFileName().c_str());
    tinyxml2::XMLElement *pMapElem = pElem->GetDocument()->NewElement("map");
    pElem->InsertEndChild(pMapElem);
    m_mapActs.at(m_uiCurrentAct)->getScene(m_uiCurrentScene)->writeToXMLElement(pMapElem, eStyle);
  }
  else {
    tinyxml2::XMLElement *pSceneElem = pElem->GetDocument()->NewElement("scene");
    pElem->InsertEndChild(pSceneElem);
    m_mapActs.at(m_uiCurrentAct)->getScene(m_uiCurrentScene)->writeToXMLElement(pSceneElem, eStyle);
  }
}
void CScreenplay::readFromXMLElement(const tinyxml2::XMLElement *pElem) {
  clear(); 

  m_eScreenplayType = EnumAttribute(pElem, "type", SCREENPLAY_SINGLE_MAP);
  m_uiCurrentAct = pElem->IntAttribute("currentAct");
  m_uiCurrentScene = pElem->IntAttribute("currentScene");
  m_uiNextAct = pElem->IntAttribute("nextAct");
  m_uiNextScene = pElem->IntAttribute("nextScene");

  if (m_eScreenplayType == SCREENPLAY_SINGLE_MAP) {
    loadSingleMap(std::shared_ptr<CMapPack>(new CMapPack(Attribute(pElem, "map_filename"))));
    CGameState::getSingleton().setMapPack(m_pMapPack);
    m_Fader.setVisible(false);
    loadAct(m_uiNextAct, m_uiNextScene);
    m_Fader.setVisible(true);
    m_mapActs.at(m_uiCurrentAct)->getScene(m_uiCurrentScene)->readFromXMLElement(pElem->FirstChildElement("map"));
  }
}
