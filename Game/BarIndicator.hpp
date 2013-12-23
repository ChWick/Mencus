#ifndef BARINDiCATOR_HPP
#define BARINDiCATOR_HPP

#include "Sprite.hpp"
#include "Util.hpp"

class CBarIndicator : public CSprite {
private:
  CSprite m_Indicator;
  bool m_bVisible;
  Ogre::Real m_fValue;		//!< Value in [0,1]
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
		Ogre::Vector2(1, 0.125)),
    m_bVisible(false),
    m_fValue(1) {

    setTexture(getHUDTexturePath("indicator_box"));
    m_Indicator.setTexture(getHUDTexturePath("indicator_red"));
  }

  void update(Ogre::Real tpf) {
    if (m_bVisible) {
      m_Indicator.setPosition(getPosition());
      m_Indicator.getTexture().setTexturePosRight(m_fValue);
      m_Indicator.setSize(Ogre::Vector2(m_fValue * m_vSize.x, m_vSize.y));
      CSprite::update(tpf);
      m_Indicator.update(tpf);
    }
  }
  void setValue(Ogre::Real fValue) {m_fValue = fValue;}
  void show() {m_bVisible = true;}
  void hide() {m_bVisible = false;}
  
};

#endif
