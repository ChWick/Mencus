#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "Screenplay.hpp"
#include <vector>
#include <OgreString.h>
#include <OgreVector2.h>
#include "Sprite.hpp"
#include "SpriteTransformPipeline.hpp"
#include "ogre2d-main.hpp"
#include "InputListener.hpp"
#include "PauseListener.hpp"

class CMap;

class CVideo : public CScene, public CInputListener, public CPauseListener {
public:
  class CPicture {
  public:
    class CEffect {
    public:
      virtual ~CEffect() {}
      virtual void update(Ogre::Real tpf, Ogre::Real fTimePos, CPicture *pPicture) = 0;
    };
    class CEffectScale : public CEffect {
    public:
      enum EScaleType {
        ST_LINEAR,
        ST_QUADRATIC,
      };
    private:
      const Ogre::Vector2 m_vCenter;
      const Ogre::Vector2 m_vStartScale;
      const Ogre::Vector2 m_vEndScale;
      EScaleType m_eScaleType;
    public:
      CEffectScale(const Ogre::Vector2 &vCenter, const Ogre::Vector2 &vStartScale, const Ogre::Vector2 &vEndScale, EScaleType eScaleType)
        : m_vCenter(vCenter), m_vStartScale(vStartScale), m_vEndScale(vEndScale), m_eScaleType(eScaleType) {
      }
      virtual void update(Ogre::Real tpf, Ogre::Real fTimePos, CPicture *pPicture);
    };
  private:
    std::vector<CEffect*> m_vEffects;
    const Ogre::Real m_fDuration;
    const Ogre::String m_sFile;
    CSprite m_Sprite;
    Ogre::Vector2 m_vDrawPos;
    Ogre::Vector2 m_vDrawSize;
    bool m_bStopped;
  public:
    CPicture(const Ogre::String &sFile, const Ogre::Real fDuration, CMap &map);
    ~CPicture();

    void init();
    void exit();

    void start();
    void stop();
    
    CSprite &getSprite() {return m_Sprite;}

    void addEffect(CEffect *pEffect) {m_vEffects.push_back(pEffect);}
    Ogre::Real getDuration() const {return m_fDuration;}
    void update(Ogre::Real tpf, Ogre::Real fPassedTime);
    void render();

    friend class CEffectScale;
  };
  class CPart {
  private:
    Ogre::String m_sAudioFile;
    std::vector<CPicture*> m_vPictures;
    size_t m_iCurrentPicture;
    Ogre::Real m_fTimer;
    CVideo *m_pVideo;
  public:
    CPart(CVideo *pVideo, const Ogre::String &sAudioFile);
    ~CPart();

    void addPicture(CPicture *pPicture) {m_vPictures.push_back(pPicture);}
    void update(Ogre::Real tpf);
    void render();
    bool isFinished() const {return m_iCurrentPicture == m_vPictures.size();}

    void init();
    void exit();

    void start();
    void stop();
  };
private:
  std::vector<CPart*> m_vParts;
  size_t m_iCurrentPart;
  CScreenplayListener *m_pListener;
  Ogre2dManager m_SpriteManager;

  bool m_bPaused;
public:
  CVideo(const CAct &act, unsigned int uiID, CScreenplayListener *pListener);
  ~CVideo();

  void addPart(CPart *pPart) {m_vParts.push_back(pPart);}

public:
  void init();
  void exit();

  virtual void start();
  virtual void stop();

  void nextPart();
  void update(Ogre::Real tpf);

  bool keyPressed( const OIS::KeyEvent &arg );
  bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  bool touchPressed(const OIS::MultiTouchEvent& evt);

  Ogre2dManager *getSpriteManager() {return &m_SpriteManager;}

  void videoPauseChanged(bool bPause);
};

#endif // VIDEO_HPP
