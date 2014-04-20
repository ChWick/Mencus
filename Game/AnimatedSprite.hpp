#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include "Sprite.hpp"
#include <tinyxml2.h>

typedef vector<CSpriteTexture> tAnimationSequence;
typedef vector<tAnimationSequence> tAnimationSequences;
typedef std::string (*tGetPathFunctionType)(std::string, int);

class CAnimatedSprite : public CSprite {
private:
  bool m_bAnimationPaused;
protected:
  tAnimationSequences m_AnimationSequences;
  unsigned int m_uiCurrentAnimationSequence;
  unsigned int m_uiCurrentAnimationTexture;

  Ogre::Real m_fAnimationTimer;
  Ogre::Real m_fAnimationSpeed;

  tGetPathFunctionType m_pDefaultGetPathFunction;
public:
  CAnimatedSprite(CMap &map,
		  const std::string &sID,
		  CEntity *pParent,
		  const CSpriteTransformPipeline *pTransformPipeline,
		  Ogre2dManager *pSpriteManager,
		  const Ogre::Vector2 &vPosition,
		  const Ogre::Vector2 &vSize,
		  const Ogre::Vector2 &vScale = Ogre::Vector2::UNIT_SCALE,
		  const Ogre::Radian radRotation = Ogre::Radian(0));

  CAnimatedSprite(CMap &map,
		  CEntity *pParent,
		  const CSpriteTransformPipeline *pTransformPipeline,
		  Ogre2dManager *pSpriteManager,
		  const tinyxml2::XMLElement *pElem,
		  const Ogre::Vector2 &vSize = Ogre::Vector2::UNIT_SCALE);

  ~CAnimatedSprite();

  void init(Ogre::Real fAnimationSpeed, unsigned int uiNumberOfAnimation = 1);

  unsigned int getCurrentAnimationSequence() const {return m_uiCurrentAnimationSequence;}
  unsigned int getCurrentAnimationTexture() const {return m_uiCurrentAnimationTexture;}


  void setDefaultGetPath(tGetPathFunctionType pFunc) {m_pDefaultGetPathFunction = pFunc;}
  void setNumberOfAnimationSquences(unsigned int size);
  unsigned int addAnimationSequence();
  void setCurrentAnimationSequence(unsigned int uiCurrentAnimationSequence);
  CSpriteTexture &addTextureToAnimationSequence(unsigned int uiAnimSequence, Ogre::TexturePtr pTexture);
  CSpriteTexture &addTextureToAnimationSequence(unsigned int uiAnimSequence, string sFilename);
  void setCurrentAnimationTexture(unsigned int uiTextIndex);

  void setupAnimation(unsigned int uiAnimSequence,
		      const std::string &sName,
		      int uiCount,
		      CSpriteTexture::EMirrorTypes eMirrorType = CSpriteTexture::MIRROR_NONE,
		      tGetPathFunctionType pGetPathFunction = NULL);
  void setupAnimation(unsigned int uiAnimSequence,
		      const std::string &sName,
		      const std::vector<int> &uiIDs,
		      CSpriteTexture::EMirrorTypes eMirrorType = CSpriteTexture::MIRROR_NONE,
		      tGetPathFunctionType pGetPathFunction = NULL);

  void changeCurrentAnimationSequence(unsigned int uiCurrentAnimationSequence, bool reset = false);

  CSpriteTexture &getTexture(unsigned int uiCurrentAnimationSequence, unsigned int uiTextIndex) {return m_AnimationSequences[uiCurrentAnimationSequence][uiTextIndex];}

  virtual void update(Ogre::Real tpf);
  virtual void render(Ogre::Real tpf);

  void setPauseAnimation(bool b) {m_bAnimationPaused = b;}

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;
protected:
  virtual void animationTextureChangedCallback(unsigned int uiOldText, unsigned int uiNewText) {}

protected:
};

#endif
