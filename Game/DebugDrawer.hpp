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
struct SSwitchEntry;

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

  void draw(const CTile *pTile, unsigned int uiTT, float fAlpha = 0.5);
  void draw(const CTile *pTile, EDebugTextures dt = DT_PINK) const;
  void draw(const CBoundingBox2d &bb) const;
  void draw(const CLink &link) const;
  void draw(const SSwitchEntry &switchEntry, float alpha = 1) const;
};

#endif
