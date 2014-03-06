#ifndef _SHADER_MANAGER_HPP_
#define _SHADER_MANAGER_HPP_

#include <OgreSingleton.h>
#include <OgreHighLevelGpuProgramManager.h>

enum OgreRendererShaderTypes {
  ORST_UNUSED,           //!< no shader used
  ORST_GLSL,             //!< default open gl shader (not implemented)
  ORST_GLSL_CORE,        //!< default open gl (v3) shader (not implemented)
  ORST_HLSL,             //!< default directX (cg) shader (not implemented)
  ORST_GLSLES,           //!< glsles (e.g. for android devices)
};


class CShaderManager : public Ogre::Singleton<CShaderManager> {
private:
  Ogre::RenderSystem *m_pRenderSystem;
  OgreRendererShaderTypes m_ShaderTypes;

  Ogre::HighLevelGpuProgramPtr m_SpriteVertexShader;
  Ogre::HighLevelGpuProgramPtr m_SpritePixelShader;
  Ogre::GpuProgramParametersSharedPtr m_SpriteVertexShaderParameters;
  Ogre::GpuProgramParametersSharedPtr m_SpritePixelShaderParameters;
public:
  static CShaderManager& getSingleton(void);
  static CShaderManager* getSingletonPtr(void);

  CShaderManager(Ogre::RenderSystem *pRenderSystem);

  bool isUsingShaders() {return m_ShaderTypes != ORST_UNUSED;}

  void bindSpriteShaders();
  void updateSpriteShaderParams();

  Ogre::HighLevelGpuProgramPtr getSpriteVertexShader() {return m_SpriteVertexShader;}
  Ogre::HighLevelGpuProgramPtr getSpritePixelShader() {return m_SpritePixelShader;}

private:
  void setupSpriteShader();
  void cleanupSpriteShader();
  
};


#endif
