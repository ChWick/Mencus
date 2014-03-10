#include "ShaderManager.hpp"
#include <OgreRenderSystem.h>

namespace ShaderPrograms {
  // -------------------------------------------------------------------------------
  // shader scripts
  static Ogre::String
  S_glsl_compat_vs_source(
			  "void main(void)"
			  "{"
			  "    gl_TexCoord[0] = gl_MultiTexCoord0;"
			  "    gl_FrontColor = gl_Color;"
			  "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
			  "}"
			  );
  static Ogre::String
  S_glsl_compat_ps_source(
			  "uniform sampler2D texture0;"
			  "void main(void)"
			  "{"
			  "    gl_FragColor = texture2D(texture0, gl_TexCoord[0].st) * gl_Color;"
			  "}"
			  );
  static Ogre::String
  S_glsles_vs_source(
		     "#version 100 \n"
		     "precision highp float;"
		     "attribute vec4 vertex;"
		     "attribute vec2 uv0;"
		     "attribute vec4 colour;"

		     "varying vec2 exTexCoord;"
		     "varying vec4 exColour;"

		     "uniform mat4 modelViewPerspMatrix;"

		     "void main()"
		     "{"
		     "  exTexCoord = uv0;"
		     "  exColour = vec4(1.0, 1.0, 1.0, 1.0);"
		     "  gl_Position = modelViewPerspMatrix * vertex;"
		     "}"
		     );

  static Ogre::String
  S_glsles_ps_source(
		     "#version 100 \n"
		     "precision highp float;"
		     "uniform sampler2D texture0;"
		     "varying vec2 exTexCoord;"
		     "varying vec4 exColour;"

		     "void main(void)"
		     "{"
		     "  gl_FragColor = texture2D( texture0, exTexCoord ) * exColour;"
		     "}"
		     );
}

// --------------------------------------------------------------------------------

template<> CShaderManager* Ogre::Singleton<CShaderManager>::msSingleton = 0;

CShaderManager* CShaderManager::getSingletonPtr(void)
{
  return msSingleton;
}
CShaderManager& CShaderManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

CShaderManager::CShaderManager(Ogre::RenderSystem *pRenderSystem)
  : m_pRenderSystem(pRenderSystem), m_ShaderTypes(ORST_UNUSED) {
  setupSpriteShader();
}

void CShaderManager::setupSpriteShader() {
  using namespace ShaderPrograms;

#ifdef USE_SPRITE_SHADER
  Ogre::String shaderLanguage;
  if (m_pRenderSystem->getName().compare(0, 8, "OpenGL 3") == 0) {
    m_ShaderTypes = ORST_GLSL_CORE;
    shaderLanguage = "glsl";
    Ogre::LogManager::getSingleton().logMessage("Using glsl core shader");
  }
  else if (Ogre::HighLevelGpuProgramManager::getSingleton().isLanguageSupported("glsles")) {
    m_ShaderTypes = ORST_GLSLES;
    shaderLanguage = "glsles";
    Ogre::LogManager::getSingleton().logMessage("Using glsles shader");
  }
  else if (Ogre::HighLevelGpuProgramManager::getSingleton().isLanguageSupported("glsl")) {
    m_ShaderTypes = ORST_GLSL;
    shaderLanguage = "glsl";
    Ogre::LogManager::getSingleton().logMessage("Using glsl shader");
  }
  else {
    throw Ogre::Exception(0, "No shader types supported!", __FILE__);
  }

  // Create vertex shader
  m_SpriteVertexShader = Ogre::HighLevelGpuProgramManager::getSingleton().
    createProgram("__ogre2dmanager_vs__",
		  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		  shaderLanguage, Ogre::GPT_VERTEX_PROGRAM);
  m_SpriteVertexShader->setParameter("entry_point", "main");
  m_SpriteVertexShader->setParameter("profiles", shaderLanguage);

  switch (m_ShaderTypes) {
  case ORST_GLSLES:
    m_SpriteVertexShader->setParameter("target", "glsles");
    m_SpriteVertexShader->setSource(S_glsles_vs_source);
    break;
  case ORST_GLSL:
    m_SpriteVertexShader->setParameter("target", "arbvp1");
    m_SpriteVertexShader->setSource(S_glsl_compat_vs_source);
    break;
  default:
    break;
  }

  m_SpriteVertexShader->load();

  // Create pixel shader
  m_SpritePixelShader = Ogre::HighLevelGpuProgramManager::getSingleton().
    createProgram("__ogre2dmanager_ps__",
		  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		  shaderLanguage, Ogre::GPT_FRAGMENT_PROGRAM);
  m_SpritePixelShader->setParameter("entry_point", "main");
  m_SpritePixelShader->setParameter("profiles", shaderLanguage);

  switch (m_ShaderTypes) {
  case ORST_GLSLES:
    m_SpritePixelShader->setParameter("target", "glsles");
    m_SpritePixelShader->setSource(S_glsles_ps_source);
    break;
  case ORST_GLSL:
    m_SpritePixelShader->setParameter("target", "arbvp1");
    m_SpritePixelShader->setSource(S_glsl_compat_ps_source);
    break;
  default:
    break;
  }

  m_SpritePixelShader->load();

  m_SpriteVertexShaderParameters = m_SpriteVertexShader->createParameters();
  m_SpritePixelShaderParameters = m_SpritePixelShader->createParameters();
#endif
}
void CShaderManager::bindSpriteShaders() {
#ifdef USE_SPRITE_SHADER
  if (Ogre::GpuProgram* prog = m_SpriteVertexShader->_getBindingDelegate())
    m_pRenderSystem->bindGpuProgram(prog);

  if (Ogre::GpuProgram* prog = m_SpritePixelShader->_getBindingDelegate())
    m_pRenderSystem->bindGpuProgram(prog);
#else
  m_pRenderSystem->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
  m_pRenderSystem->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
#endif
}
void CShaderManager::updateSpriteShaderParams() {
#ifdef USE_SPRITE_SHADER
  switch (m_ShaderTypes) {
  case ORST_GLSLES:
  case ORST_GLSL_CORE:
    m_SpriteVertexShaderParameters->
      setNamedConstant("modelViewPerspMatrix", Ogre::Matrix4::IDENTITY);    
    
    m_pRenderSystem->
      bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM,
			       m_SpriteVertexShaderParameters,
			       Ogre::GPV_ALL);
    // no break!
  case ORST_GLSL:
    m_SpritePixelShaderParameters->
      setNamedConstant("texture0", 0);
    
    m_pRenderSystem->
      bindGpuProgramParameters(Ogre::GPT_FRAGMENT_PROGRAM,
			       m_SpritePixelShaderParameters,
			       Ogre::GPV_ALL);
    break;
  default:
    break;
  }
#endif
}
