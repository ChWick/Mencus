#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include "Sprite.hpp"

typedef vector<CSpriteTexture> tAnimationSequence;
typedef vector<tAnimationSequence> tAnimationSequences;
typedef std::string (*tGetPathFunctionType)(std::string, int);

class CAnimatedSprite : public CSprite {
protected:
  tAnimationSequences m_AnimationSequences;
  unsigned int m_uiCurrentAnimationSequence;
  unsigned int m_uiCurrentAnimationTexture;

  Ogre::Real m_fAnimationTimer;
  Ogre::Real m_fAnimationSpeed;

  tGetPathFunctionType m_pDefaultGetPathFunction;
public:
  CAnimatedSprite(const CSpriteTransformPipeline *pTransformPipeline, Ogre2dManager *pSpriteManager, const Ogre::Vector2 &vPosition, const Ogre::Vector2 &vSize, const Ogre::Vector2 &vScale = Ogre::Vector2::UNIT_SCALE, const Ogre::Radian radRotation = Ogre::Radian(0));

  void init(Ogre::Real fAnimationSpeed, unsigned int uiNumberOfAnimation = 1);

  unsigned int getCurrentAnimationSequence() const {return m_uiCurrentAnimationSequence;}
  unsigned int getCurrentAnimationTexture() const {return m_uiCurrentAnimationTexture;}

  
  void setDefaultGetPath(tGetPathFunctionType pFunc) {m_pDefaultGetPathFunction = pFunc;}
  void setNumberOfAnimationSquences(unsigned int size);
  unsigned int addAnimationSequence();
  void setCurrentAnimationSequence(unsigned int uiCurrentAnimationSequence);
  CSpriteTexture &addTextureToCurrentAnimationSequence(Ogre::TexturePtr pTexture);
  CSpriteTexture &addTextureToCurrentAnimationSequence(string sFilename);
  void setCurrentAnimationTexture(unsigned int uiTextIndex);
  
  void setupAnimation(unsigned int uiAnimSequence,
		      const std::string &sName,
		      int uiCount,
		      EMirrorTypes eMirrorType = MIRROR_NONE,
		      tGetPathFunctionType pGetPathFunction = NULL);
  void setupAnimation(unsigned int uiAnimSequence,
		      const std::string &sName,
		      const std::vector<int> &uiIDs,
		      EMirrorTypes eMirrorType = MIRROR_NONE,
		      tGetPathFunctionType pGetPathFunction = NULL);

  void changeCurrentAnimationSequence(unsigned int uiCurrentAnimationSequence, bool reset = false);

  CSpriteTexture &getTexture(unsigned int uiCurrentAnimationSequence, unsigned int uiTextIndex) {return m_AnimationSequences[uiCurrentAnimationSequence][uiTextIndex];}

  virtual void update(Ogre::Real tpf);

protected:
  virtual void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText) {}

protected:
};

#endif
