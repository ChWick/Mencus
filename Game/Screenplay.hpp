#ifndef SCREENPLAY_HPP
#define SCREENPLAY_HPP

#include <tinyxml2.h>
#include <OgreString.h>
#include <CEGUI/String.h>

class CMap;

class CScene {
public:
  enum ESceneTypes {
    ST_LEVEL,
    ST_INSTRUCTION,

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
  virtual void start() = 0;
  virtual void stop() = 0;

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

public:
  CLevel(unsigned int uiID, const Ogre::String &sFilename)
    : CScene(uiID, ST_LEVEL),
      m_sFilename(sFilename),
      m_pMap(NULL) {
  }
  virtual ~CLevel();

  virtual void start();
  virtual void stop();
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
  ~CAct();

  void addScene(CScene *pScene) {m_mapScenes[pScene->getID()] = pScene;}

  CScene *getScene(unsigned int id) {return m_mapScenes.at(id);}
  CScene *operator[](unsigned int id) {return m_mapScenes[id];}

  virtual void start();
  virtual void stop();
};

class CScreenplay {
private:
  const Ogre::String m_sLevelDir;

  std::map<unsigned int, CAct*> m_mapActs;

  unsigned int m_uiCurrentAct;
  unsigned int m_uiCurrentScene;

  CScene *m_pOldScene;
public:
  CScreenplay();
  ~CScreenplay();

  void loadAct(unsigned int uiActId, unsigned int uiSceneId = 1);
private:
  void parse(const Ogre::String &sFilename);
};
#endif // SCREENPLAY_HPP
