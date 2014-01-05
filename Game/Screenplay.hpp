#ifndef SCREENPLAY_HPP
#define SCREENPLAY_HPP

#include <tinyxml2.h>
#include <OgreString.h>
#include <CEGUI/String.h>
#include "ScreenplayListener.hpp"

class CMap;

class CScene {
public:
  enum ESceneTypes {
    ST_LEVEL,
    ST_INSTRUCTION,
    ST_VIDEO,

    ST_COUNT
  };
private:
  const unsigned int m_uiID;
  const ESceneTypes m_eSceneType;

protected:
  CScene(unsigned int uiID, ESceneTypes eSceneTypes)
    : m_uiID(uiID),
      m_eSceneType(eSceneTypes) {
  }

public:
  virtual ~CScene() {}

  virtual void start() = 0;
  virtual void stop() = 0;

  virtual void update(Ogre::Real tpf) {}

  unsigned int getID() const {return m_uiID;}
  ESceneTypes getType() const {return m_eSceneType;}
};

class CInstructions : public CScene {
private:
  const CEGUI::String m_sText;

public:
  CInstructions(unsigned int uiID, const CEGUI::String &sText)
    : CScene(uiID, ST_INSTRUCTION),
      m_sText(sText) {
    }

    virtual void start();
    virtual void stop();
};

class CLevel : public CScene {
private:
  const Ogre::String m_sFilename;
  CMap *m_pMap;
  CScreenplayListener *m_pScreenplayListener;

public:
  CLevel(unsigned int uiID, const Ogre::String &sFilename, CScreenplayListener *pScreenplayListener)
    : CScene(uiID, ST_LEVEL),
      m_sFilename(sFilename),
      m_pMap(NULL),
      m_pScreenplayListener(pScreenplayListener) {
  }
  virtual ~CLevel();

  virtual void start();
  virtual void stop();
  void update(Ogre::Real tpf);
};

class CAct {
private:
  const unsigned int m_uiID;
  const Ogre::String m_sDirectory;
  std::map<unsigned int, CScene *> m_mapScenes;

public:
  CAct(unsigned int uiID, const Ogre::String &sDirectory)
    : m_uiID(uiID),
      m_sDirectory(sDirectory) {
  }
  CAct(const CAct &src)
    : m_uiID(src.m_uiID),
      m_sDirectory(src.m_sDirectory) {
  }
  virtual ~CAct();

  void addScene(CScene *pScene) {m_mapScenes[pScene->getID()] = pScene;}
  size_t getSceneCount() const {return m_mapScenes.size();}

  CScene *getScene(unsigned int id) {return m_mapScenes.at(id);}
  CScene *operator[](unsigned int id) {return m_mapScenes[id];}

  virtual void start();
  virtual void stop();
};

class CScreenplay : public CScreenplayListener {
private:
  const Ogre::String m_sLevelDir;

  std::map<unsigned int, CAct*> m_mapActs;

  unsigned int m_uiCurrentAct;
  unsigned int m_uiCurrentScene;

  unsigned int m_uiNextAct;
  unsigned int m_uiNextScene;

  CScene *m_pCurrentScene;
  CScene *m_pOldScene;
public:
  CScreenplay();
  ~CScreenplay();

  void setNextAct(unsigned int uiActId, unsigned int uiSceneId) {m_uiNextAct = uiActId; m_uiNextScene = uiSceneId;}

  void keyForContinueInstructionsPressed();
  void playerExitsMap();
  void videoFinished();

  void update(Ogre::Real tpf);

private:

  void loadAct(unsigned int uiActId, unsigned int uiSceneId = 1);
  void parse(const Ogre::String &sFilename);
};
#endif // SCREENPLAY_HPP
