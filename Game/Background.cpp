#include "Background.hpp"
#include "SpriteTransformPipeline.hpp"

const Ogre::Real BACKGROUND_SIZE = 1.333333f;
const Ogre::Real BACKGROUND_BOTTOM_OFFSET = 0.666667f;

CBackground::CBackground(Ogre2dManager *pSpriteManager, const Ogre::Vector2 &vCameraPosition, const Ogre::String &sTexture)
  : CSprite(&CDefaultSpriteTransformPipeline::INSTANCE, pSpriteManager, Ogre::Vector2(-1, -1), Ogre::Vector2(BACKGROUND_SIZE * SCREEN_RATIO, BACKGROUND_SIZE)),
    m_vCameraPosition(vCameraPosition) {
  setTexture(sTexture);
}
void CBackground::render(Ogre::Real tpf) {
  //setTextureCoords(Ogre::Vector2(0, 0), Ogre::Vector2(1.5 / SCREEN_RATIO, 1.5));
  int iTimesX = static_cast<int>(m_vCameraPosition.x / 8.f);
  int iTimesY = static_cast<int>(m_vCameraPosition.y / 8.f);

  Ogre::Vector2 vPos = 0.125f * (Ogre::Vector2(iTimesX, iTimesY) * 8 - m_vCameraPosition);
  vPos.y /= SCREEN_RATIO;
  vPos -= Ogre::Vector2(1, 1 + BACKGROUND_BOTTOM_OFFSET);

  Ogre::Real fYInit = vPos.y;
  for (; vPos.x < 1; vPos.x += m_vSize.x) {
    for (; vPos.y < 1; vPos.y += m_vSize.y) {
      setPosition(vPos);
      CSprite::render(tpf);
    }
    vPos.y = fYInit;
  }
}
