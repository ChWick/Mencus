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

#ifndef FADER_HPP
#define FADER_HPP

#include <Ogre.h>

#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
#include "OgreOverlaySystem.h"
#endif

#include "ShaderManager.hpp"
#include "XMLHelper.hpp"
#include "OutputStyle.hpp"

class CFaderCallback {
public:
  virtual void fadeInCallback() {}
  virtual void fadeOutCallback() {}
};

class CFader {
public:
  enum EFadeOperation {
    FADE_NONE,
    FADE_IN,
    FADE_OUT,
  };

private:
  Ogre::Real m_fAlpha;
  Ogre::Real m_fCurrentDuration;
  Ogre::Real m_fTotalDuration;
  CFaderCallback *m_pFaderCallback;
  Ogre::TextureUnitState *m_pTextUnitState;
  Ogre::Overlay *m_pOverlay;
  EFadeOperation m_eFadeOperation;

  Ogre::GpuProgramParametersSharedPtr m_SpriteVertexShaderParameters;
  Ogre::GpuProgramParametersSharedPtr m_SpritePixelShaderParameters;
public:
  CFader(CFaderCallback *pCallback = nullptr)
    :
    m_fAlpha(0),
    m_fCurrentDuration(0),
    m_fTotalDuration(0),
    m_pFaderCallback(pCallback),
    m_eFadeOperation(FADE_NONE) {
    init();
  }
  CFader(const tinyxml2::XMLElement *pElem, CFaderCallback *pCallback = nullptr)
    : m_fAlpha(XMLHelper::RealAttribute(pElem, "fade_alpha", 0)),
      m_fCurrentDuration(XMLHelper::RealAttribute(pElem, "fade_current_duration")),
      m_fTotalDuration(XMLHelper::RealAttribute(pElem, "fade_total_duration")),
      m_pFaderCallback(pCallback),
      m_eFadeOperation(XMLHelper::EnumAttribute<EFadeOperation>(pElem, "fade_operation", FADE_NONE)) {
      init();
  }

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const {
    using namespace XMLHelper;

    if (eStyle == OS_FULL) {
      SetAttribute(pElem, "fade_alpha", m_fAlpha);
      SetAttribute(pElem, "fade_current_duration", m_fCurrentDuration);
      SetAttribute(pElem, "fade_total_duration", m_fTotalDuration);
      SetAttribute(pElem, "fade_operation", m_eFadeOperation);
    }
  }

  bool isFading() const {return m_eFadeOperation != FADE_NONE;}
  void setVisible(bool bShow) {if (bShow){m_pOverlay->show();} else {m_pOverlay->hide();}}

  void startFadeIn(Ogre::Real fDuration) {
    if (fDuration < 0) fDuration = -fDuration;

    m_fAlpha = 1.0;
    m_fTotalDuration = fDuration;
    m_fCurrentDuration = fDuration;
    m_eFadeOperation = FADE_IN;
    m_pOverlay->show();
  }
  void startFadeOut(Ogre::Real fDuration) {
    if (fDuration < 0) fDuration = -fDuration;

    m_fAlpha = 0;
    m_fTotalDuration = fDuration;
    m_fCurrentDuration = 0;
    m_eFadeOperation = FADE_OUT;
    m_pOverlay->show();
  }

  void fade(Ogre::Real tpf) {
    if (m_eFadeOperation != FADE_NONE && m_pTextUnitState) {
      m_pTextUnitState->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, m_fAlpha);

      if (m_eFadeOperation == FADE_IN) {
        m_fCurrentDuration -= tpf;
        m_fAlpha = m_fCurrentDuration / m_fTotalDuration;
        if (m_fAlpha < 0.0) {
          m_pOverlay->hide();
          m_eFadeOperation = FADE_NONE;
          if (m_pFaderCallback) {
            m_pFaderCallback->fadeInCallback();
          }
        }
      }
      else if (m_eFadeOperation == FADE_OUT) {
        m_fCurrentDuration += tpf;
        m_fAlpha = m_fCurrentDuration / m_fTotalDuration;
        if (m_fAlpha > 1.0) {
          m_eFadeOperation = FADE_NONE;
          if (m_pFaderCallback) {
            m_pFaderCallback->fadeOutCallback();
          }
        }
      }
    }
#ifdef USE_SPRITE_SHADER
    m_SpritePixelShaderParameters->setNamedConstant("colour", Ogre::ColourValue(1, 1, 1, m_fAlpha));
#endif
  }

private:
  void init() {
    Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().getByName("FadeMaterial");
#ifdef USE_SPRITE_SHADER
    CShaderManager::getSingleton().addAlphaBlendTextureShader(matptr);
    Ogre::Pass *pPass = matptr->getTechnique(0)->getPass(0);
    m_SpritePixelShaderParameters = pPass->getFragmentProgram()->createParameters();
    m_SpriteVertexShaderParameters = pPass->getVertexProgram()->createParameters();
    pPass->setFragmentProgramParameters(m_SpritePixelShaderParameters);
    pPass->setVertexProgramParameters(m_SpriteVertexShaderParameters);
#endif
    m_pTextUnitState = matptr->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    m_pOverlay = Ogre::OverlayManager::getSingleton().getByName("FadeOverlay");
    m_pOverlay->hide();
  }
};


#endif
