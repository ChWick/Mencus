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

#ifndef _SETTINGS_HPP_
#define _SETTINGS_HPP_

#include <OgreSingleton.h>
#include "InputDefines.hpp"

struct SInputSettings {
  SInputSettings();

#ifdef INPUT_TOUCH
  // touch input
  float m_fTouchButtonSize;
#endif
  float m_fMapEditorButtonSize;
};
struct SVideoSettings {
  SVideoSettings();

  float m_fHUDSize;
};

class CSettings : public Ogre::Singleton<CSettings> {
private:
  SInputSettings m_InputSettings;
  SVideoSettings m_VideoSettings;
public:
    static CSettings &getSingleton();
    static CSettings *getSingletonPtr();
    
  CSettings();
  ~CSettings();

  SInputSettings &getInputSettings() {return m_InputSettings;}
  SVideoSettings &getVideoSettings() {return m_VideoSettings;}  

  void writeToFile();
private:

  void readFromFile();
};

#endif
