#include "Video.hpp"
#include "OgreRoot.h"

void CVideo::CPicture::CEffectScale::update(Ogre::Real tpf, Ogre::Real fTimePos) {

}


CVideo::CPicture::CPicture(const Ogre::String &sFile, const Ogre::Real fDuration, Ogre2dManager *p2dManager)
  : m_fDuration(fDuration),
    m_sFile(sFile),
    m_Sprite(&CDefaultSpriteTransformPipeline::INSTANCE, p2dManager, Ogre::Vector2(-1, -1), Ogre::Vector2(2, 2)) {
  m_Sprite.setTexture(sFile);
}
void CVideo::CPicture::update(Ogre::Real tpf, Ogre::Real fPassedTime) {
  for (auto pEffect : m_vEffects) {
      pEffect->update(tpf, fPassedTime / m_fDuration);
  }
  m_Sprite.update(tpf);
}

CVideo::CVideo(unsigned int uiID, CScreenplayListener *pListener)
  : CScene(uiID, ST_VIDEO),
    m_pListener(pListener) {
    m_SpriteManager.init(Ogre::Root::getSingleton().getSceneManager("MainSceneManager"), Ogre::RENDER_QUEUE_OVERLAY, true);
}
CVideo::~CVideo() {
  for (auto pPart : m_vParts) {delete pPart;}
  m_SpriteManager.end();
}
void CVideo::start() {
  m_iCurrentPart = 0;
  m_vParts[m_iCurrentPart]->start();
}
void CVideo::stop() {
  m_vParts[m_iCurrentPart]->stop();
}
bool CVideo::frameStarted(const Ogre::FrameEvent& evt) {
  m_vParts[m_iCurrentPart]->update(evt.timeSinceLastFrame);
  if (m_vParts[m_iCurrentPart]->isFinished()) {
    m_vParts[m_iCurrentPart]->stop();
    ++m_iCurrentPart;
    if (m_iCurrentPart == m_vParts.size()) {
      m_pListener->videoFinished();
    }
    else {
      m_vParts[m_iCurrentPart]->start();
    }
  }
  return true;
}
