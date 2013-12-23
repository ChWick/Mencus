#ifndef FADER_HPP
#define FADER_HPP

#include <Ogre.h>

class CFaderCallback {
public:
  virtual void fadeInCallback() {}
  virtual void fadeOutCallback() {}
};

class CFader {
public:
  enum EFadeOperation {
    FADE_NONE,
    FADE_IN,
    FADE_OUT,
  };

private:
  Ogre::Real m_fAlpha;
  Ogre::Real m_fCurrentDuration;
  Ogre::Real m_fTotalDuration;
  CFaderCallback *m_pFaderCallback;
  Ogre::TextureUnitState *m_pTextUnitState;
  Ogre::Overlay *m_pOverlay;
  EFadeOperation m_eFadeOperation;
public:
  CFader(CFaderCallback *pCallback = NULL) 
    :
    m_fAlpha(0),
    m_fCurrentDuration(0),
    m_fTotalDuration(0),
    m_pFaderCallback(pCallback),
    m_eFadeOperation(FADE_NONE) {
    Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().getByName("FadeMaterial");
    
    m_pTextUnitState = matptr->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    m_pOverlay = Ogre::OverlayManager::getSingleton().getByName("FadeOverlay");
    m_pOverlay->hide();
  }
  void startFadeIn(Ogre::Real fDuration) {
    if (fDuration < 0) fDuration = -fDuration;

    m_fAlpha = 1.0;
    m_fTotalDuration = fDuration;
    m_fCurrentDuration = fDuration;
    m_eFadeOperation = FADE_IN;
    m_pOverlay->show();
  }
  void startFadeOut(Ogre::Real fDuration) {
    if (fDuration < 0) fDuration = -fDuration;
    
    m_fAlpha = 0;
    m_fTotalDuration = fDuration;
    m_fCurrentDuration = 0;
    m_eFadeOperation = FADE_OUT;
    m_pOverlay->show();
  }

  void fade(Ogre::Real tpf) {
    if (m_eFadeOperation != FADE_NONE && m_pTextUnitState) {
      m_pTextUnitState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, m_fAlpha);

      if (m_eFadeOperation == FADE_IN) {
	m_fCurrentDuration -= tpf;
	m_fAlpha = m_fCurrentDuration / m_fTotalDuration;
	if (m_fAlpha < 0.0) {
	  m_pOverlay->hide();
	  m_eFadeOperation = FADE_NONE;
	  if (m_pFaderCallback) {
	    m_pFaderCallback->fadeInCallback();
	  }
	}
      }
      else if (m_eFadeOperation == FADE_OUT) {
	m_fCurrentDuration += tpf;
	m_fAlpha = m_fCurrentDuration / m_fTotalDuration;
	if (m_fAlpha > 1.0) {
	  m_eFadeOperation = FADE_NONE;
	  if (m_pFaderCallback) {
	    m_pFaderCallback->fadeOutCallback();
	  }
	}
      }
    }
  }
};


#endif
