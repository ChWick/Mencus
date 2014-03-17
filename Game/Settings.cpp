#include "Settings.hpp"
#include "FileManager.hpp"
#include "XMLHelper.hpp"
#include <tinyxml2.h>

using namespace XMLHelper;
using namespace tinyxml2;

template<> CSettings *Ogre::Singleton<CSettings>::msSingleton = 0;

const std::string SETTINGS_FILE_PATH("settings.xml");

// default values
SInputSettings::SInputSettings() {
#ifdef INPUT_TOUCH
  m_fTouchButtonSize = 85;
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
  if (CFileManager::openFile(stream, SETTINGS_FILE_PATH)) {
    stream << header;
    stream << text;
    stream.close();
  }
}
