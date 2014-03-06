#include "Video.hpp"
#include "OgreRoot.h"
#include <OgreTextureManager.h>
#include "PauseManager.hpp"

void CVideo::CPicture::CEffectScale::update(Ogre::Real tpf, Ogre::Real fTimePos, CPicture *pPicture) {
  Ogre::Real fScaleFactor = fTimePos;
  switch (m_eScaleType) {
  case ST_LEVEL:
    break;
  case ST_QUADRATIC:
    fScaleFactor *= fTimePos;
    break;
  }
  Ogre::Vector2 vScale(m_vStartScale + (m_vEndScale - m_vStartScale) * fScaleFactor);

  pPicture->m_vDrawSize *= vScale;
  pPicture->m_vDrawPos *= vScale;
  pPicture->m_vDrawPos -= m_vCenter * fScaleFactor;
}


CVideo::CPicture::CPicture(const Ogre::String &sFile, const Ogre::Real fDuration, Ogre2dManager *p2dManager)
  : m_fDuration(fDuration),
    m_sFile(sFile),
    m_Sprite(&CDefaultSpriteTransformPipeline::INSTANCE, p2dManager, Ogre::Vector2(-1, -1), Ogre::Vector2(2, 2)),
    m_bStopped(true) {
}
CVideo::CPicture::~CPicture() {
  for (auto pEffect : m_vEffects) {delete pEffect;}
}
void CVideo::CPicture::start() {
  m_bStopped = false;
}
void CVideo::CPicture::stop() {
  m_bStopped = true;
}
void CVideo::CPicture::init() {
  // set texture
  m_Sprite.setTexture(m_sFile);
}
void CVideo::CPicture::exit() {
  // delete video texutre
  Ogre::TextureManager::getSingleton().remove(m_sFile);
}
void CVideo::CPicture::update(Ogre::Real tpf, Ogre::Real fPassedTime) {
  if (m_bStopped) {return;}

  m_vDrawPos = Ogre::Vector2(-1, -1);
  m_vDrawSize = Ogre::Vector2(2, 2);
  for (auto pEffect : m_vEffects) {
      pEffect->update(tpf, fPassedTime / m_fDuration, this);
  }
  m_Sprite.setPosition(m_vDrawPos);
  m_Sprite.setSize(m_vDrawSize);
  //m_Sprite.update(tpf);
}
void CVideo::CPicture::render() {
  m_Sprite.render(0);
}

CVideo::CPart::CPart(CVideo *pVideo, const Ogre::String &sAudioFile)
  : m_sAudioFile(sAudioFile), m_iCurrentPicture(0), m_fTimer(0), m_pVideo(pVideo) {
}
CVideo::CPart::~CPart() {
  for (auto pPicture : m_vPictures) {delete pPicture;}
}
void CVideo::CPart::update(Ogre::Real tpf) {
  m_fTimer += tpf;
  m_vPictures[m_iCurrentPicture]->update(tpf, m_fTimer);

  if (m_fTimer >= m_vPictures[m_iCurrentPicture]->getDuration()) {
    m_iCurrentPicture++;
    m_fTimer = 0;
  }
}
void CVideo::CPart::render() {
  if (m_iCurrentPicture < m_vPictures.size())
    m_vPictures[m_iCurrentPicture]->render();
}
void CVideo::CPart::start() {
  m_iCurrentPicture = 0;
  m_fTimer = 0;
  for (auto pPicture : m_vPictures) {
    pPicture->start();
  }
}
void CVideo::CPart::stop() {
  for (auto pPicture : m_vPictures) {
    pPicture->stop();
  }
}
void CVideo::CPart::init() {
  for (auto pPicture : m_vPictures) {
    pPicture->init();
  }
}
void CVideo::CPart::exit() {
  for (auto pPicture : m_vPictures) {
    pPicture->exit();
  }
}

CVideo::CVideo(const CAct &act, unsigned int uiID, CScreenplayListener *pListener)
  : CScene(act, uiID, ST_VIDEO),
    m_pListener(pListener),
    m_bPaused(false) {
}
CVideo::~CVideo() {
  exit();
  for (auto pPart : m_vParts) {delete pPart;}
}
void CVideo::init() {
  CPauseManager::getSingleton().addListener(this);
  m_SpriteManager.init(Ogre::Root::getSingleton().getSceneManager("MainSceneManager"), Ogre::RENDER_QUEUE_OVERLAY, false);
  CInputListenerManager::getSingleton().addInputListener(this);
  setInputListenerEnabled(false);
  for (auto pPart : m_vParts) {pPart->init();}
}
void CVideo::exit() {
  for (auto pPart : m_vParts) {pPart->exit();}
  CPauseManager::getSingleton().removeListener(this);
  CInputListenerManager::getSingleton().removeInputListener(this);
  m_SpriteManager.end();
}
void CVideo::start() {
  setInputListenerEnabled(true);
  m_iCurrentPart = 0;
  m_vParts[m_iCurrentPart]->start();
}
void CVideo::stop() {
  setInputListenerEnabled(false);
  if (m_iCurrentPart != m_vParts.size()) {
    m_vParts[m_iCurrentPart]->stop();
  }
}
void CVideo::nextPart() {
  m_vParts[m_iCurrentPart]->stop();
  if (m_iCurrentPart + 1 == m_vParts.size()) {
    m_pListener->videoFinished();
  }
  else {
    ++m_iCurrentPart;
    m_vParts[m_iCurrentPart]->start();
  }
}
void CVideo::update(Ogre::Real tpf) {
  if (m_iCurrentPart == m_vParts.size()) {
      // draw last part
      m_vParts.back()->render();
      return;
  }

  m_vParts[m_iCurrentPart]->render();

  if (m_vParts[m_iCurrentPart]->isFinished()) {
    nextPart();
  }
  else {
    if (!m_bPaused) {
      m_vParts[m_iCurrentPart]->update(tpf);
    }
  }
}
bool CVideo::keyPressed( const OIS::KeyEvent &arg ) {
  nextPart();

  return true;
}
bool CVideo::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  nextPart();

  return true;
}
bool CVideo::touchPressed(const OIS::MultiTouchEvent& evt) {
  nextPart();

  return true;
}
void CVideo::videoPauseChanged(bool bPause) {
  m_bPaused = bPause;
  setInputListenerEnabled(!bPause);
}
