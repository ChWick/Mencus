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

#ifndef SPRITE_TRANSFORM_PIPELINE_HPP
#define SPRITE_TRANSFORM_PIPELINE_HPP

#include <Ogre.h>

class CSpriteTransformPipeline {
public:
  virtual Ogre::Vector2 transformPosition(const Ogre::Vector2 &vPosition) const = 0;
  virtual bool isVisible(const Ogre::Vector2 &vPosition) const ;
  virtual bool isVisible(const Ogre::Vector2 &vStart, const Ogre::Vector2 &vEnd) const;
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

