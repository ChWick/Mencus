#include "SpriteTransformPipeline.hpp"

const CDefaultSpriteTransformPipeline CDefaultSpriteTransformPipeline::INSTANCE;
const CBackgroundSpriteTransformPipeline CBackgroundSpriteTransformPipeline::INSTANCE;

bool CSpriteTransformPipeline::isVisible(const Ogre::Vector2 &vPosition) const {
  return -1 <= vPosition.y && -1 <= vPosition.x && 1 >= vPosition.y && 1 >= vPosition.x;
}
bool CSpriteTransformPipeline::isVisible(const Ogre::Vector2 &vStart,
					 const Ogre::Vector2 &vEnd) const {
  return !(vEnd.x < -1 || vEnd.y < -1 || vStart.x > 1 || vStart.y > 1);
}
