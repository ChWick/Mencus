#ifndef DEBUG_DRAWER_H
#define DEBUG_DRAWER_H

#include <Ogre.h>
#include <string>
#include "DebugDefines.hpp"

class CTile;
class CBoundingBox2d;
class CSprite;
class CSpriteTransformPipeline;
class Ogre2dManager;
class CLink;

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

  CDebugDrawer(const CSpriteTransformPipeline *pTransformPipeline,
    Ogre2dManager *pSpriteManager);
  ~CDebugDrawer();

  void draw(const CTile *pTile, EDebugTextures dt = DT_PINK) const;
  void draw(const CBoundingBox2d &bb) const;
  void draw(const CLink &link) const;
};

#endif
