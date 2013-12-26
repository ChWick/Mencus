#include "AnimatedSprite.hpp"

CAnimatedSprite::CAnimatedSprite(const CSpriteTransformPipeline *pTransformPipeline, Ogre2dManager *pSpriteManager, const Ogre::Vector2 &vPosition, const Ogre::Vector2 &vSize, const Ogre::Vector2 &vScale, const Ogre::Radian radRotation)
  : CSprite(pTransformPipeline, pSpriteManager, vPosition, vSize, vScale, radRotation),
    m_uiCurrentAnimationTexture(0),
    m_uiCurrentAnimationSequence(0),
    m_bAnimationPaused(false) {
  m_fAnimationTimer = 0;
  m_fAnimationSpeed = 1.0 / 20;
}
void CAnimatedSprite::init(Ogre::Real fAnimationSpeed, unsigned int uiNumberOfAnimation) {
  m_AnimationSequences.resize(uiNumberOfAnimation);
  setCurrentAnimationSequence(0);
}
void CAnimatedSprite::setNumberOfAnimationSquences(unsigned int size) {
  m_AnimationSequences.resize(size);
}
unsigned int CAnimatedSprite::addAnimationSequence() {
  m_AnimationSequences.resize(m_AnimationSequences.size() + 1);
}
void CAnimatedSprite::setCurrentAnimationSequence(unsigned int uiCurrentAnimationSequence) {
  m_uiCurrentAnimationSequence = uiCurrentAnimationSequence;
}
CSpriteTexture &CAnimatedSprite::addTextureToCurrentAnimationSequence(Ogre::TexturePtr pTexture) {
  m_AnimationSequences[m_uiCurrentAnimationSequence].push_back(pTexture);
  return m_AnimationSequences[m_uiCurrentAnimationSequence].back();
}
CSpriteTexture &CAnimatedSprite::addTextureToCurrentAnimationSequence(string sFilename) {
  setTexture(sFilename);
  return addTextureToCurrentAnimationSequence(Ogre::TextureManager::getSingleton().getByName(sFilename));
}
void CAnimatedSprite::setCurrentAnimationTexture(unsigned int uiTextIndex) {
  m_uiCurrentAnimationTexture = uiTextIndex;
}
void CAnimatedSprite::changeCurrentAnimationSequence(unsigned int uiCurrentAnimationSequence, bool bReset) {
  if (m_uiCurrentAnimationSequence == uiCurrentAnimationSequence) {
    if (bReset) {
      m_uiCurrentAnimationTexture = 0;
      m_fAnimationTimer = 0;
    }
    return;
  }
  m_uiCurrentAnimationSequence = uiCurrentAnimationSequence;
  m_uiCurrentAnimationTexture = 0;
  m_fAnimationTimer = 0;
}
void CAnimatedSprite::setupAnimation(unsigned int uiAnimSequence,
				     const std::string &sName,
				     int uiCount,
				     CSpriteTexture::EMirrorTypes eMirrorType,
				     tGetPathFunctionType pGetPathFunction) {
  tGetPathFunctionType pFunc = (pGetPathFunction == NULL) ? m_pDefaultGetPathFunction : pGetPathFunction;
  assert(pFunc);
  setCurrentAnimationSequence(uiAnimSequence);
  if (uiCount <= 0) {
    addTextureToCurrentAnimationSequence((*pFunc)(sName, -1)).mirror(eMirrorType);
  }
  else {
    for (int i = 0; i < uiCount; ++i) {
      addTextureToCurrentAnimationSequence((*pFunc)(sName, i + 1)).mirror(eMirrorType);
    }
  }
}
void CAnimatedSprite::setupAnimation(unsigned int uiAnimSequence,
				     const std::string &sName,
				     const std::vector<int> &uiIDs,
				     CSpriteTexture::EMirrorTypes eMirrorType,
				     tGetPathFunctionType pGetPathFunction) {
  tGetPathFunctionType pFunc = (pGetPathFunction == NULL) ? m_pDefaultGetPathFunction : pGetPathFunction;
  assert(pFunc);
  setCurrentAnimationSequence(uiAnimSequence);
  for (auto i : uiIDs) {
    addTextureToCurrentAnimationSequence((*pFunc)(sName, i + 1)).mirror(eMirrorType);
  }
}
void CAnimatedSprite::update(Ogre::Real tpf) {
  if (!m_bAnimationPaused) {
    m_fAnimationTimer += tpf;
    if (m_fAnimationTimer >= m_fAnimationSpeed) {
      unsigned int uiOldTexture = m_uiCurrentAnimationTexture;
      ++m_uiCurrentAnimationTexture;
      if (m_uiCurrentAnimationTexture >= m_AnimationSequences[m_uiCurrentAnimationSequence].size()) {
        m_uiCurrentAnimationTexture = 0;
      }

      animationTextureChangedCallback(uiOldTexture, m_uiCurrentAnimationTexture);

      m_fAnimationTimer = 0;
    }
  }

  setTextureToDraw(&m_AnimationSequences[m_uiCurrentAnimationSequence][m_uiCurrentAnimationTexture]);
  CSprite::update(tpf);
}
