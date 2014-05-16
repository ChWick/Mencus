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
