#include "ShaderManager.hpp"
#include <OgreRenderSystem.h>

// -------------------------------------------------------------------------------
// shader scripts
static Ogre::String S_glsles_vs_source(
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
    "  exColour = colour;"
    "  gl_Position = modelViewPerspMatrix * vertex;"
    "}"
);

static Ogre::String S_glsles_ps_source(
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
  Ogre::String shaderLanguage;
  if (Ogre::HighLevelGpuProgramManager::getSingleton().isLanguageSupported("glsles")) {
    m_ShaderTypes = ORST_GLSLES;
    shaderLanguage = "glsles";
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
  default:
    break;
  }

  m_SpritePixelShader->load();

  m_SpriteVertexShaderParameters = m_SpriteVertexShader->createParameters();
  m_SpritePixelShaderParameters = m_SpritePixelShader->createParameters();
}
void CShaderManager::bindSpriteShaders() {
  if (isUsingShaders()) {
    if (Ogre::GpuProgram* prog = m_SpriteVertexShader->_getBindingDelegate())
      m_pRenderSystem->bindGpuProgram(prog);

    if (Ogre::GpuProgram* prog = m_SpritePixelShader->_getBindingDelegate())
      m_pRenderSystem->bindGpuProgram(prog);

    updateSpriteShaderParams();
  }
  else {
    m_pRenderSystem->unbindGpuProgram(Ogre::GPT_VERTEX_PROGRAM);
    m_pRenderSystem->unbindGpuProgram(Ogre::GPT_FRAGMENT_PROGRAM);
  }
}
void CShaderManager::updateSpriteShaderParams() {
  if (!isUsingShaders()) {return;}

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
}
