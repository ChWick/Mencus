#ifndef SPRITE_TRANSFORM_PIPELINE_HPP
#define SPRITE_TRANSFORM_PIPELINE_HPP

#include <Ogre.h>

class CSpriteTransformPipeline {
public:
  virtual Ogre::Vector2 transformPosition(const Ogre::Vector2 &vPosition) const = 0;
};

class CDefaultSpriteTransformPipeline : public CSpriteTransformPipeline {
public:
  static const CDefaultSpriteTransformPipeline INSTANCE;

  Ogre::Vector2 transformPosition(const Ogre::Vector2 &vPosition) const {
    return vPosition;
  }
};

class CBackgroundSpriteTransformPipeline : public CSpriteTransformPipeline {
public:
  static const CBackgroundSpriteTransformPipeline INSTANCE;

  Ogre::Vector2 transformPosition(const Ogre::Vector2 &vPosition) const {
    Ogre::Vector2 vTransformed(vPosition);
    vTransformed.y = -vTransformed.y;
    return vTransformed;
  }
};
#endif

