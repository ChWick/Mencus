#ifndef BARINDiCATOR_HPP
#define BARINDiCATOR_HPP

#include "Sprite.hpp"
#include "Util.hpp"

class CBarIndicator : public CSprite {
private:
  CSprite m_Indicator;
public:
  CBarIndicator(const CSpriteTransformPipeline *pTransformPipeline,
		Ogre2dManager *pSpriteManager)
    :
    CSprite(pTransformPipeline,
	    pSpriteManager,
	    Ogre::Vector2::ZERO,
	    Ogre::Vector2(1, 0.125)),
    m_Indicator(pTransformPipeline,
		pSpriteManager,
		Ogre::Vector2::ZERO,
		Ogre::Vector2(1, 0.125)) {

    setTexture(getHUDTexturePath("indicator_box"));
    m_Indicator.setTexture(getHUDTexturePath("indicator_box"));
  }

  void update(Ogre::Real tpf) {
    m_Indicator.setPosition(getPosition());
    CSprite::update(tpf);
    m_Indicator.update(tpf);
  }
  
};

#endif
