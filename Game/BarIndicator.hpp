/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef BARINDiCATOR_HPP
#define BARINDiCATOR_HPP

#include "Sprite.hpp"
#include "Util.hpp"
#include "IDGenerator.hpp"

class CMap;

class CBarIndicator : public CSprite {
private:
  CSprite m_Indicator;
  bool m_bVisible;
  Ogre::Real m_fValue;		//!< Value in [0,1]
public:
  CBarIndicator(CMap &map,
		CEntity *pParent,
		const CSpriteTransformPipeline *pTransformPipeline,
		Ogre2dManager *pSpriteManager)
    :
    CSprite(map,
	    CIDGenerator::nextID("BarIndicator_"),
	    pParent,
	    pTransformPipeline,
	    pSpriteManager,
	    Ogre::Vector2::ZERO,
	    Ogre::Vector2(1, 0.125)),
    m_Indicator(map,
		CIDGenerator::nextID("BarIndicator_indicator_"),
		this,
		pTransformPipeline,
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
    }
  }
  void render(Ogre::Real tpf) {
    if (m_bVisible) {
      CSprite::render(tpf);
      m_Indicator.render(tpf);
    }
  }
  void setValue(Ogre::Real fValue) {m_fValue = fValue;}
  void show() {m_bVisible = true;}
  void hide() {m_bVisible = false;}

};

#endif
