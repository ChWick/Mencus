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

#ifndef DEBUG_DRAWER_H
#define DEBUG_DRAWER_H

#include <Ogre.h>
#include <string>
#include "DebugDefines.hpp"

class CTile;
class CSprite;
class CBoundingBox2d;
class CSpriteTransformPipeline;
class Ogre2dManager;
class CLink;
class CEvent;
class CMap;

class CDebugDrawer : public Ogre::Singleton<CDebugDrawer> {
public:
  enum EDebugTextures {
    DT_PINK,
    DT_BLUE,
    DT_GREEN,

    DT_COUNT,
  };
private:
  CSprite *m_pDebugSprite;
  std::string m_DebugTextures[DT_COUNT];
public:
  static CDebugDrawer &getSingleton();
  static CDebugDrawer *getSingletonPtr();

  CDebugDrawer(CMap &map);
  ~CDebugDrawer();

  void draw(const CTile *pTile, unsigned int uiTT, float fAlpha = 0.5);
  void draw(const CTile *pTile, EDebugTextures dt = DT_PINK) const;
  void draw(const CBoundingBox2d &bb) const;
  void draw(const CLink &link) const;
  void draw(const CEvent *pEvent, float alpha = 1) const;
};

#endif
