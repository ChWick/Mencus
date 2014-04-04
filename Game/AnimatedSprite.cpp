#include "AnimatedSprite.hpp"
#include "XMLHelper.hpp"

using namespace XMLHelper;

CAnimatedSprite::CAnimatedSprite(CMap &map,
				 const Ogre::Vector2 &vPosition,
				 const Ogre::Vector2 &vSize,
				 const Ogre::Vector2 &vScale,
				 const Ogre::Radian radRotation)
  : CSprite(map, vPosition, vSize, vScale, radRotation),
    m_bAnimationPaused(false),
    m_uiCurrentAnimationSequence(0),
    m_uiCurrentAnimationTexture(0) {
  m_fAnimationTimer = 0;
  m_fAnimationSpeed = 1.0 / 20;
}
CAnimatedSprite::CAnimatedSprite(CMap &map,
				 const tinyxml2::XMLElement *pElem,
				 const Ogre::Vector2 &vSize) 
  : CSprite(map, pElem, vSize),
    m_bAnimationPaused(BoolAttribute(pElem, "as_animation_paused", false)),
    m_uiCurrentAnimationSequence(IntAttribute(pElem, "as_current_as", 0)),
    m_uiCurrentAnimationTexture(IntAttribute(pElem, "as_current_at", 0)),
    m_fAnimationTimer(RealAttribute(pElem, "as_timer", 0)),
    m_fAnimationSpeed(1.0 / 20) {
}

void CAnimatedSprite::init(Ogre::Real fAnimationSpeed, unsigned int uiNumberOfAnimation) {
  m_AnimationSequences.resize(uiNumberOfAnimation);
}
void CAnimatedSprite::setNumberOfAnimationSquences(unsigned int size) {
  m_AnimationSequences.resize(size);
}
unsigned int CAnimatedSprite::addAnimationSequence() {
  m_AnimationSequences.resize(m_AnimationSequences.size() + 1);
  return m_AnimationSequences.size();
}
void CAnimatedSprite::setCurrentAnimationSequence(unsigned int uiCurrentAnimationSequence) {
  m_uiCurrentAnimationSequence = uiCurrentAnimationSequence;
}
CSpriteTexture &CAnimatedSprite::addTextureToAnimationSequence(unsigned int uiAnimSequence, Ogre::TexturePtr pTexture) {
  m_AnimationSequences[uiAnimSequence].push_back(pTexture);
  return m_AnimationSequences[uiAnimSequence].back();
}
CSpriteTexture &CAnimatedSprite::addTextureToAnimationSequence(unsigned int uiAnimSequence, string sFilename) {
  setTexture(sFilename);
  return addTextureToAnimationSequence(uiAnimSequence, Ogre::TextureManager::getSingleton().getByName(sFilename));
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
  if (uiCount <= 0) {
    addTextureToAnimationSequence(uiAnimSequence, (*pFunc)(sName, -1)).mirror(eMirrorType);
  }
  else {
    for (int i = 0; i < uiCount; ++i) {
      addTextureToAnimationSequence(uiAnimSequence, (*pFunc)(sName, i + 1)).mirror(eMirrorType);
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
  for (auto i : uiIDs) {
    addTextureToAnimationSequence(uiAnimSequence, (*pFunc)(sName, i + 1)).mirror(eMirrorType);
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
  CSprite::update(tpf);
}
void CAnimatedSprite::render(Ogre::Real tpf) {
  setTextureToDraw(&m_AnimationSequences[m_uiCurrentAnimationSequence][m_uiCurrentAnimationTexture]);
  CSprite::render(tpf);
}
void CAnimatedSprite::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
  if (eStyle == OS_FULL) {
    pElem->SetAttribute("as_animation_paused", m_bAnimationPaused);
    pElem->SetAttribute("as_current_as", m_uiCurrentAnimationSequence);
    pElem->SetAttribute("as_current_at", m_uiCurrentAnimationTexture);
    pElem->SetAttribute("as_timer", m_fAnimationTimer);
  }

  CSprite::writeToXMLElement(pElem, eStyle);
}
