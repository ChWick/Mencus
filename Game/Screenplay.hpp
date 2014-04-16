#ifndef SCREENPLAY_HPP
#define SCREENPLAY_HPP

#include <tinyxml2.h>
#include <OgreString.h>
#include <CEGUI/String.h>
#include "ScreenplayListener.hpp"
#include "Fader.hpp"
#include "PauseCaller.hpp"
#include "PauseListener.hpp"
#include "InputListener.hpp"
#include <memory>
#include "Statistics.hpp"
#include "OutputStyle.hpp"

class CMap;
class CScreenplay;
class CAct;
class CMapInfo;

class CScene {
public:
  enum ESceneTypes {
    ST_LEVEL,
    ST_INSTRUCTION,
    ST_VIDEO,

    ST_COUNT
  };
protected:
  const CAct &m_Act;
private:
  const unsigned int m_uiID;
  const ESceneTypes m_eSceneType;

protected:
  CScene(const CAct &act, unsigned int uiID, ESceneTypes eSceneTypes)
    : m_Act(act),
      m_uiID(uiID),
      m_eSceneType(eSceneTypes) {
  }

public:
  virtual ~CScene() {}


  virtual void init() = 0;
  virtual void exit() = 0;

  virtual void start() = 0;
  virtual void stop() = 0;

  virtual void update(Ogre::Real tpf) {}
  virtual void render(Ogre::Real tpf) {}

  unsigned int getID() const {return m_uiID;}
  ESceneTypes getType() const {return m_eSceneType;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {}
  virtual void readFromXMLElement(const tinyxml2::XMLElement *pElem) {}
};

class CInstructions : public CScene {
private:
  const CEGUI::String m_sText;

public:
  CInstructions(const CAct &act, unsigned int uiID, const CEGUI::String &sText)
    : CScene(act, uiID, ST_INSTRUCTION),
      m_sText(sText) {
    }

    virtual void start();
    virtual void stop();
    virtual void init() {}
    virtual void exit() {}
};

class CLevel : public CScene {
private:
  std::shared_ptr<const CMapInfo> m_pMapInfo;
  CMap *m_pMap;
  CScreenplayListener *m_pScreenplayListener;
  SStatistics m_Statistics;
  
public:
  CLevel(const CAct &act, unsigned int uiID, std::shared_ptr<const CMapInfo> pMapInfo, CScreenplayListener *pScreenplayListener)
    : CScene(act, uiID, ST_LEVEL),
      m_pMapInfo(pMapInfo),
      m_pMap(NULL),
      m_pScreenplayListener(pScreenplayListener) {
  }
  virtual ~CLevel();

  virtual void start();
  virtual void stop();
  virtual void init();
  virtual void exit();
  void update(Ogre::Real tpf);
  void render(Ogre::Real tpf);

  std::shared_ptr<const CMapInfo> getMapInfo() const {return m_pMapInfo;}
  const SStatistics &getStatistics() const {return m_Statistics;}
  SStatistics &getStatistics() {return m_Statistics;}
  CMap *getMap() {return m_pMap;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;
  virtual void readFromXMLElement(const tinyxml2::XMLElement *pElem);
};

class CAct {
private:
  const CScreenplay &m_Screenplay;
  const unsigned int m_uiID;
  const Ogre::String m_sDirectory;
  std::map<unsigned int, CScene *> m_mapScenes;

public:
  CAct(const CScreenplay &screenplay, unsigned int uiID, const Ogre::String &sDirectory)
    : m_Screenplay(screenplay),
      m_uiID(uiID),
      m_sDirectory(sDirectory) {
  }
  CAct(const CAct &src)
    : m_Screenplay(src.m_Screenplay),
      m_uiID(src.m_uiID),
      m_sDirectory(src.m_sDirectory) {
  }
  virtual ~CAct();

  void addScene(CScene *pScene) {m_mapScenes[pScene->getID()] = pScene;}
  size_t getSceneCount() const {return m_mapScenes.size();}

  CScene *getScene(unsigned int id) {return m_mapScenes.at(id);}
  const CScene *getScene(unsigned int id) const {return m_mapScenes.at(id);}
  CScene *operator[](unsigned int id) {return m_mapScenes[id];}
  const CScene *operator[](unsigned int id) const {return m_mapScenes.at(id);}

  const CScreenplay &getScreenplay() const {return m_Screenplay;}

  virtual void start();
  virtual void stop();
};

enum EScreenplayTypes {
  SCREENPLAY_CAMPAIN = 0,
  SCREENPLAY_SINGLE_MAP,
  SCREENPLAY_NONE,
};
class CScreenplay :
  public CScreenplayListener,
  public CFaderCallback,
  public CPauseCaller,
  public CInputListener,
  public CPauseListener {
private:
  EScreenplayTypes m_eScreenplayType;
  const Ogre::String m_sResourceGroup;

  std::map<unsigned int, CAct*> m_mapActs;

  unsigned int m_uiCurrentAct;
  unsigned int m_uiCurrentScene;

  unsigned int m_uiNextAct;
  unsigned int m_uiNextScene;

  CScene *m_pCurrentScene;
  CScene *m_pOldScene;

  std::shared_ptr<const CMapInfo> m_pMapInfo;

  CFader m_Fader;
  bool m_bPaused;

  // framerate computation
  static const int TPF_AVERAGE_COUNT = 10;
  Ogre::Real m_fAveragedTpf;
  std::list<Ogre::Real> m_lLastFrames;
public:
  CScreenplay();
  virtual ~CScreenplay();

  void loadSingleMap(std::shared_ptr<const CMapInfo> pMapInfo);

  void clear();

  void setNextAct(unsigned int uiActId, unsigned int uiSceneId) {m_uiNextAct = uiActId; m_uiNextScene = uiSceneId;}

  void keyForContinueInstructionsPressed();
  void playerExitsMap();
  void videoFinished();

  void update(Ogre::Real tpf);

  const Ogre::String &getResourceGroup() const {return m_sResourceGroup;}
  // CFader
  void fadeInCallback();
  void fadeOutCallback();

  // CInputListener
  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  // CPauseListener
  virtual void screenplayPauseChanged(bool bPause) {m_bPaused = bPause; setInputListenerEnabled(!bPause);}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;
  virtual void readFromXMLElement(const tinyxml2::XMLElement *pElem);
private:

  void loadAct(unsigned int uiActId, unsigned int uiSceneId = 1);
  void parse(const Ogre::String &sFilename, const Ogre::String &sResourceGroup);

  void toNextScene();
};
#endif // SCREENPLAY_HPP
