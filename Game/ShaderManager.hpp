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

#ifndef _SHADER_MANAGER_HPP_
#define _SHADER_MANAGER_HPP_

#include "GlobalBuildDefines.hpp"

#if MENCUS_ENABLE_SHADER == 1
#define USE_SPRITE_SHADER
#endif

#include <OgreSingleton.h>
#include <OgreHighLevelGpuProgramManager.h>
#include <OgreColourValue.h>

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

  bool m_bSpriteParamsChanged;
  Ogre::ColourValue m_cLastSpriteColor;
  Ogre::String m_sShaderLanguage;
public:
  static CShaderManager& getSingleton(void);
  static CShaderManager* getSingletonPtr(void);

  CShaderManager(Ogre::RenderSystem *pRenderSystem);

  bool isUsingShaders() {return m_ShaderTypes != ORST_UNUSED;}

  void bindSpriteShaders();
  void updateSpriteShaderParams();

  void setSpriteColor(const Ogre::ColourValue &col);
  Ogre::HighLevelGpuProgramPtr getSpriteVertexShader() {return m_SpriteVertexShader;}
  Ogre::HighLevelGpuProgramPtr getSpritePixelShader() {return m_SpritePixelShader;}

  void addAlphaBlendTextureShader(Ogre::MaterialPtr matptr);
private:
  void setupSpriteShader();
  void cleanupSpriteShader();
};


#endif
