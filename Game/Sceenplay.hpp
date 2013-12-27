#ifndef SCREENPLAY_HPP
#define SCREENPLAY_HPP

class CScene {
public:
  enum ESceneTypes {
    ST_LEVEL,

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

class CLevel : public CScene {
private:
  const Ogre::String m_sFilename;

public:
  CLevel(unsigned int uiID, const Ogre::String &sFilename)
    : CScene(uiID, ST_LEVEL),
      m_sFilename(sFilemame) {
  }

  virtual void start();
  virtual void stop();
};

class CAct {
private:
  const unsigned int m_uiID;
  const Ogre::String m_sDirectory;

public:
  CAct(unsigned int uiID, const Ogre::String &sDirectory)
    : m_uiID(uiID),
      m_sDirectory(sDirectory) {
  }
  CAct(const CAct &src)
    : m_uiID(src.m_uiID),
      m_sDirectory(src.m_sDirectory) {
  }

  virtual void start();
  virtual void stop();
};

class CScreenplay {
private:
  std::map<unsigned int, CAct> m_mapActs;
public:
  CScreenplay();
private:
  void parse(const Ogre::String &sFilename);
};
#endif // SCREENPLAY_HPP
