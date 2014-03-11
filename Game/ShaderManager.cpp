#include "ShaderManager.hpp"
#include <OgreRenderSystem.h>
#include <OgrePass.h>
#include <OgreTechnique.h>

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
			  "uniform vec4 colour;"
			  "void main(void)"
			  "{"
			  "    gl_FragColor = texture2D(texture0, gl_TexCoord[0].st) * colour;"
			  "}"
			  );
  static Ogre::String
  S_glsles_vs_source(
		     "#version 100 \n"
		     "precision highp float;"
		     "attribute vec4 vertex;"
		     "attribute vec2 uv0;"

		     "varying vec2 exTexCoord;"
		     "varying vec4 exColour;"

		     "uniform mat4 modelViewPerspMatrix;"

		     "void main()"
		     "{"
		     "  exTexCoord = uv0;"
		     "  gl_Position = modelViewPerspMatrix * vertex;"
		     "}"
		     );

  static Ogre::String
  S_glsles_ps_source(
		     "#version 100 \n"
		     "precision highp float;"
		     "uniform sampler2D texture0;"
		     "uniform vec4 colour;"
		     "varying vec2 exTexCoord;"

		     "void main(void)"
		     "{"
		     "  gl_FragColor = texture2D( texture0, exTexCoord ) * colour;"
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
  m_cLastSpriteColor = Ogre::ColourValue::White;

  if (m_pRenderSystem->getName().compare(0, 8, "OpenGL 3") == 0) {
    m_ShaderTypes = ORST_GLSL_CORE;
    m_sShaderLanguage = "glsl";
    Ogre::LogManager::getSingleton().logMessage("Using glsl core shader");
  }
  else if (Ogre::HighLevelGpuProgramManager::getSingleton().isLanguageSupported("glsles")) {
    m_ShaderTypes = ORST_GLSLES;
    m_sShaderLanguage = "glsles";
    Ogre::LogManager::getSingleton().logMessage("Using glsles shader");
  }
  else if (Ogre::HighLevelGpuProgramManager::getSingleton().isLanguageSupported("glsl")) {
    m_ShaderTypes = ORST_GLSL;
    m_sShaderLanguage = "glsl";
    Ogre::LogManager::getSingleton().logMessage("Using glsl shader");
  }
  else {
    throw Ogre::Exception(0, "No shader types supported!", __FILE__);
  }

  setupSpriteShader();
}
void CShaderManager::setupSpriteShader() {
  using namespace ShaderPrograms;
 

  // Create vertex shader
  m_SpriteVertexShader = Ogre::HighLevelGpuProgramManager::getSingleton().
    createProgram("__ogre2dmanager_vs__",
		  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		  m_sShaderLanguage, Ogre::GPT_VERTEX_PROGRAM);
  m_SpriteVertexShader->setParameter("entry_point", "main");
  m_SpriteVertexShader->setParameter("profiles", m_sShaderLanguage);

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
		  m_sShaderLanguage, Ogre::GPT_FRAGMENT_PROGRAM);
  m_SpritePixelShader->setParameter("entry_point", "main");
  m_SpritePixelShader->setParameter("profiles", m_sShaderLanguage);

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

  m_SpritePixelShader->getDefaultParameters()->setNamedConstant("colour", Ogre::ColourValue::White);
}
void CShaderManager::bindSpriteShaders() {
  m_bSpriteParamsChanged = true;
  if (Ogre::GpuProgram* prog = m_SpriteVertexShader->_getBindingDelegate())
    m_pRenderSystem->bindGpuProgram(prog);

  if (Ogre::GpuProgram* prog = m_SpritePixelShader->_getBindingDelegate())
    m_pRenderSystem->bindGpuProgram(prog);
}
void CShaderManager::updateSpriteShaderParams() {
  //if (!m_bSpriteParamsChanged) {return;}
  switch (m_ShaderTypes) {
  case ORST_GLSLES:   
  case ORST_GLSL_CORE:
    m_SpriteVertexShaderParameters->
      setNamedConstant("modelViewPerspMatrix", Ogre::Matrix4::IDENTITY); 
    
   
    // no break!
  case ORST_GLSL:
    m_SpritePixelShaderParameters->
      setNamedConstant("texture0", 0);
    			  
    m_SpritePixelShaderParameters->
      setNamedConstant("colour", m_cLastSpriteColor);


    m_pRenderSystem->
      bindGpuProgramParameters(Ogre::GPT_FRAGMENT_PROGRAM,
			       m_SpritePixelShaderParameters,
			       Ogre::GPV_ALL);
    m_pRenderSystem->
      bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM,
			       m_SpriteVertexShaderParameters,
			       Ogre::GPV_ALL);
    break;
  default:
    break;
  }
  m_bSpriteParamsChanged = false;
}
void CShaderManager::setSpriteColor(const Ogre::ColourValue &col) {
  if (col != m_cLastSpriteColor) {
    m_bSpriteParamsChanged = true;
    m_cLastSpriteColor = col;
  }
}
void CShaderManager::addAlphaBlendTextureShader(Ogre::MaterialPtr matptr) {
  Ogre::Pass *pPass = matptr->getTechnique(0)->getPass(0);
  if (pPass->hasVertexProgram() || pPass->hasFragmentProgram()) {
    return;
  }
  pPass->setFragmentProgram(m_SpritePixelShader->getName());
  pPass->setVertexProgram(m_SpriteVertexShader->getName());
  pPass->setFragmentProgramParameters(m_SpritePixelShaderParameters);
  pPass->setVertexProgramParameters(m_SpriteVertexShaderParameters);
}
