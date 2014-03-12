#include "Background.hpp"
#include "SpriteTransformPipeline.hpp"
#include "Util.hpp"

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
CBackground::CBackground(Ogre2dManager *pSpriteManager, const Ogre::Vector2 &vCameraPosition, const Ogre::String &sName)
  : CSprite(&CDefaultSpriteTransformPipeline::INSTANCE, pSpriteManager, Ogre::Vector2(-1, -1), Ogre::Vector2(BACKGROUND_SIZE * SCREEN_RATIO, BACKGROUND_SIZE)),
    m_vCameraPosition(vCameraPosition),
    m_sName(sName),
    m_eBackgroundType(fromString(sName)) {

  setTexture(getBackgroundTexturePath(sName));
  
}
void CBackground::render(Ogre::Real tpf) {
  //setTextureCoords(Ogre::Vector2(0, 0), Ogre::Vector2(1.5 / SCREEN_RATIO, 1.5));
  int iTimesX = static_cast<int>(m_vCameraPosition.x * BACKGROUND_CAMERA_OFFSET_SCALING_FACTOR[m_eBackgroundType] * 0.125f);
  int iTimesY = static_cast<int>(m_vCameraPosition.y * BACKGROUND_CAMERA_OFFSET_SCALING_FACTOR[m_eBackgroundType] * 0.125f);

  Ogre::Vector2 vPos = (Ogre::Vector2(iTimesX, iTimesY) - BACKGROUND_CAMERA_OFFSET_SCALING_FACTOR[m_eBackgroundType] * 0.125f * m_vCameraPosition);
  vPos.y /= SCREEN_RATIO;
  vPos -= Ogre::Vector2(1, 1 + BACKGROUND_BOTTOM_OFFSET);

  Ogre::Real fYInit = vPos.y;
  for (; vPos.x < 1; vPos.x += m_vSize.x) {
    int iCurrentRow = -1 + iTimesY;
    for (; vPos.y < 1; vPos.y += m_vSize.y) {
      if (m_eBackgroundType == BT_FOREST) {
        if (iCurrentRow < 0) {
          setTexture(getTileTexturePath(28));
        }
        else if (iCurrentRow == 0) {
          setTexture(getBackgroundTexturePath("forest"));
        }
        else {
          setTexture(getBackgroundTexturePath("clouds"));
        }
      }
      setPosition(vPos);
      CSprite::render(tpf);
      iCurrentRow++;
    }
    vPos.y = fYInit;
  }
}
