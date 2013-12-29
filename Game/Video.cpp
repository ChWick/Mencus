#include "Video.hpp"
#include "OgreRoot.h"

void CVideo::CPicture::CEffectScale::update(Ogre::Real tpf, Ogre::Real fTimePos) {

}

CVideo::CVideo(unsigned int uiID, CScreenplayListener *pListener)
  : CScene(uiID, ST_VIDEO),
    m_pListener(pListener) {
    m_SpriteManager.init(Ogre::Root::getSingleton().getSceneManager("MainSceneManager"), Ogre::RENDER_QUEUE_BACKGROUND, true);
}
void CVideo::start() {
  m_iCurrentPart = 0;
  m_vParts[m_iCurrentPart]->start();
  m_SpriteManager.end();
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
