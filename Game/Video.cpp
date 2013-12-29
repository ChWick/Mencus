#include "Video.hpp"
#include "OgreRoot.h"
#include <OgreTextureManager.h>

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
    m_Sprite(&CDefaultSpriteTransformPipeline::INSTANCE, p2dManager, Ogre::Vector2(-1, -1), Ogre::Vector2(2, 2)) {
}
CVideo::CPicture::~CPicture() {
  for (auto pEffect : m_vEffects) {delete pEffect;}
}
void CVideo::CPicture::start() {
  // set texture
  m_Sprite.setTexture(m_sFile);
}
void CVideo::CPicture::stop() {
  // delete video texutre
  Ogre::TextureManager::getSingleton().remove(m_sFile);
}
void CVideo::CPicture::update(Ogre::Real tpf, Ogre::Real fPassedTime) {
  m_vDrawPos = Ogre::Vector2(-1, -1);
  m_vDrawSize = Ogre::Vector2(2, 2);
  for (auto pEffect : m_vEffects) {
      pEffect->update(tpf, fPassedTime / m_fDuration, this);
  }
  m_Sprite.setPosition(m_vDrawPos);
  m_Sprite.setSize(m_vDrawSize);
  m_Sprite.update(tpf);
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

CVideo::CVideo(unsigned int uiID, CScreenplayListener *pListener)
  : CScene(uiID, ST_VIDEO),
    m_pListener(pListener) {
  m_SpriteManager.init(Ogre::Root::getSingleton().getSceneManager("MainSceneManager"), Ogre::RENDER_QUEUE_OVERLAY, true);
  CInputListenerManager::getSingleton().addInputListener(this);
  setInputListenerEnabled(false);
}
CVideo::~CVideo() {
  CInputListenerManager::getSingleton().removeInputListener(this);
  for (auto pPart : m_vParts) {delete pPart;}
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
  ++m_iCurrentPart;
  if (m_iCurrentPart == m_vParts.size()) {
    m_pListener->videoFinished();
  }
  else {
    m_vParts[m_iCurrentPart]->start();
  }
}
bool CVideo::frameStarted(const Ogre::FrameEvent& evt) {
  if (m_iCurrentPart == m_vParts.size()) {return true;}

  if (m_vParts[m_iCurrentPart]->isFinished()) {
    nextPart();
  }
  else {
    m_vParts[m_iCurrentPart]->update(evt.timeSinceLastFrame);
  }
  return true;
}
bool CVideo::keyPressed( const OIS::KeyEvent &arg ) {
  nextPart();

  return true;
}
