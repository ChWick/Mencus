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

#include "Settings.hpp"
#include "FileManager.hpp"
#include "XMLHelper.hpp"
#include <tinyxml2.h>
#include "GlobalBuildDefines.hpp"

using namespace XMLHelper;
using namespace tinyxml2;

template<> CSettings *Ogre::Singleton<CSettings>::msSingleton = 0;

const std::string SETTINGS_FILE_PATH("settings.xml");

// default values
SInputSettings::SInputSettings() 
  : m_fMapEditorButtonSize(50) {
#ifdef INPUT_TOUCH
  m_fTouchButtonSize = 10000; // very big number to force maximal button size as default
#endif
}
SVideoSettings::SVideoSettings()
  : 
#ifdef INPUT_TOUCH
  m_fHUDSize(1)
#else
  m_fHUDSize(0)
#endif
{
}

SSocialGamingSettings::SSocialGamingSettings() {
#if MENCUS_DEFAULT_LOGIN_TO_SOCIAL_GAMING == 1
  m_bLoginOnStart = true;
#else
  m_bLoginOnStart = false;
#endif
}

CSettings &CSettings::getSingleton() {return *msSingleton;}
CSettings *CSettings::getSingletonPtr() {return msSingleton;}

CSettings::CSettings() {
  readFromFile();
}
CSettings::~CSettings() {
  writeToFile();
}
void CSettings::readFromFile() {
  XMLDocument doc;
  doc.LoadFile(CFileManager::getValidPath(SETTINGS_FILE_PATH).c_str());
  if (doc.Error()) {
    return;
  }
  XMLElement *pRoot = doc.FirstChildElement("settings");
  if (!pRoot) {
    // no settings stored yet
    return;
  }

  // input
  XMLElement *pInput = pRoot->FirstChildElement("input");
  if (!pInput) {return;}

  m_InputSettings.m_fMapEditorButtonSize
    = RealAttribute(pInput,
		    "map_editor_button_size",
		    m_InputSettings.m_fMapEditorButtonSize);

  for (XMLElement *pElem = pInput->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement()) {
#ifdef INPUT_TOUCH
    if (strcmp(pElem->Value(), "touch") == 0) {
      m_InputSettings.m_fTouchButtonSize = RealAttribute(pElem, "button_size", m_InputSettings.m_fTouchButtonSize);
    }
#endif
  }

  // video
  XMLElement *pVideo = pRoot->FirstChildElement("video");
  if (!pVideo) {return;}

  m_VideoSettings.m_fHUDSize = RealAttribute(pVideo, "hud_size", m_VideoSettings.m_fHUDSize);
}
void CSettings::writeToFile() {
  XMLDocument doc;
  XMLElement *pSettingsElem = doc.NewElement("settings");
  doc.InsertEndChild(pSettingsElem);

  // Input
  XMLElement *pInput = doc.NewElement("input");
  pSettingsElem->InsertEndChild(pInput);

  pInput->SetAttribute("map_editor_button_size",
		       m_InputSettings.m_fMapEditorButtonSize);
  
#ifdef INPUT_TOUCH
  XMLElement *pInputTouch = doc.NewElement("touch");
  pInput->InsertEndChild(pInputTouch);
  
  pInputTouch->SetAttribute("button_size", m_InputSettings.m_fTouchButtonSize);
#endif

  // video
  XMLElement *pVideo = doc.NewElement("video");
  pSettingsElem->InsertEndChild(pVideo);
  
  pVideo->SetAttribute("hud_size", m_VideoSettings.m_fHUDSize);


  // do the output
  XMLPrinter xmlprinter;
  doc.Accept(&xmlprinter);

  std::string header("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  std::string text(xmlprinter.CStr());

  std::fstream stream;
  if (CFileManager::openFile(stream, SETTINGS_FILE_PATH, std::ofstream::out | std::ofstream::trunc)) {
    stream << header;
    stream << text;
    stream.close();
  }
}
