#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "Screenplay.hpp"
#include <vector>
#include <OgreString.h>
#include <OgreVector2.h>
#include "Sprite.hpp"
#include "SpriteTransformPipeline.hpp"
#include "ogre2d-main.hpp"

class CVideo : public CScene {
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
  public:
    CPicture(const Ogre::String &sFile, const Ogre::Real fDuration, Ogre2dManager *p2dManager);
    ~CPicture() {
      for (auto pEffect : m_vEffects) {delete pEffect;}
    }
    void addEffect(CEffect *pEffect) {m_vEffects.push_back(pEffect);}
    Ogre::Real getDuration() const {return m_fDuration;}
    void update(Ogre::Real tpf, Ogre::Real fPassedTime);

    friend CEffectScale;
  };
  class CPart {
  private:
    Ogre::String m_sAudioFile;
    std::vector<CPicture*> m_vPictures;
    size_t m_iCurrentPicture;
    Ogre::Real m_fTimer;
  public:
    CPart(const Ogre::String &sAudioFile)
      : m_sAudioFile(sAudioFile), m_iCurrentPicture(0), m_fTimer(0) {
    }
    ~CPart() {
      for (auto pPicture : m_vPictures) {delete pPicture;}
    }
    void addPicture(CPicture *pPicture) {m_vPictures.push_back(pPicture);}
    void update(Ogre::Real tpf) {
      m_fTimer += tpf;
      m_vPictures[m_iCurrentPicture]->update(tpf, m_fTimer);

      if (m_fTimer >= m_vPictures[m_iCurrentPicture]->getDuration()) {
        m_iCurrentPicture++;
        m_fTimer = 0;
      }
    }
    bool isFinished() const {return m_iCurrentPicture == m_vPictures.size();}

    void start() {m_iCurrentPicture = 0; m_fTimer = 0;}
    void stop() {}
  };
private:
  std::vector<CPart*> m_vParts;
  size_t m_iCurrentPart;
  CScreenplayListener *m_pListener;
  Ogre2dManager m_SpriteManager;
public:
  CVideo(unsigned int uiID, CScreenplayListener *pListener);
  ~CVideo();

  void addPart(CPart *pPart) {m_vParts.push_back(pPart);}

public:
  virtual void start();
  virtual void stop();

  virtual bool frameStarted(const Ogre::FrameEvent& evt);

  Ogre2dManager *getSpriteManager() {return &m_SpriteManager;}
};

#endif // VIDEO_HPP
