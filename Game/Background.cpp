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

#include "Background.hpp"
#include "SpriteTransformPipeline.hpp"
#include "Util.hpp"
#include "Map.hpp"

const Ogre::Real BACKGROUND_SIZE = 1.333333f;
const Ogre::Real BACKGROUND_BOTTOM_OFFSET = 0.666667f;

const Ogre::Real BACKGROUND_CAMERA_OFFSET_SCALING_FACTOR[CBackground::BT_COUNT] = {
  0.5,
  0.75,
  0.1,
  1
};

CBackground::ETypes CBackground::fromString(const Ogre::String &sName) {
  if (sName == "clouds") {
    return BT_CLOUDS;
  }
  else if (sName == "fog") {
    return BT_FOG;
  }
  else if (sName == "forest") {
    return BT_FOREST;
  }
  else if (sName == "stars") {
    return BT_STARS;
  }
  else {
    throw Ogre::Exception(0, "Unknown background type: " + sName, __FILE__);
  }
}
CBackground::CBackground(CMap &map,
			 const Ogre::Vector2 &vCameraPosition,
			 const Ogre::String &sName,
			 const Ogre::Vector2 &vTilesPerScreen,
			 const float &fScreenRatio)
  : CSprite(map, "Background", &map, &CDefaultSpriteTransformPipeline::INSTANCE, map.get2dManager(), Ogre::Vector2(-1, -1), Ogre::Vector2(BACKGROUND_SIZE * fScreenRatio, BACKGROUND_SIZE)),
    m_eBackgroundType(fromString(sName)),
    m_vCameraPosition(vCameraPosition),
    m_sName(sName),
    m_vTilesPerScreen(vTilesPerScreen),
    m_fScreenRatio(fScreenRatio){

  setTexture(getBackgroundTexture(sName));
  
}
void CBackground::render(Ogre::Real tpf) {
  //setTextureCoords(Ogre::Vector2(0, 0), Ogre::Vector2(1.5 / m_vScreenRatio, 1.5));
  int iTimesX = static_cast<int>(floor(m_vCameraPosition.x * BACKGROUND_CAMERA_OFFSET_SCALING_FACTOR[m_eBackgroundType] * 0.125f));
  int iTimesY = static_cast<int>(floor(m_vCameraPosition.y * BACKGROUND_CAMERA_OFFSET_SCALING_FACTOR[m_eBackgroundType] * 0.125f));

  Ogre::Vector2 vPos = (Ogre::Vector2(iTimesX, iTimesY) - BACKGROUND_CAMERA_OFFSET_SCALING_FACTOR[m_eBackgroundType] * 0.125f * m_vCameraPosition);
  vPos.y /= m_fScreenRatio;
  vPos -= Ogre::Vector2(1, 1 + BACKGROUND_BOTTOM_OFFSET);

  Ogre::Real fYInit = vPos.y;
  for (; vPos.x < 1; vPos.x += m_vSize.x) {
    int iCurrentRow = -1 + iTimesY;
    for (; vPos.y < 1; vPos.y += m_vSize.y) {
      if (m_eBackgroundType == BT_FOREST) {
        if (iCurrentRow < 0) {
          setTexture(getTileTexture(28));
        }
        else if (iCurrentRow == 0) {
          setTexture(getBackgroundTexture("forest"));
        }
        else {
          setTexture(getBackgroundTexture("clouds"));
        }
      }
      setPosition(vPos);
      CSprite::render(tpf);
      iCurrentRow++;
    }
    vPos.y = fYInit;
  }
}
