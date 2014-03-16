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
  static CSettings &getSingleton() {return *msSingleton;}
  static CSettings *getSingletonPtr() {return msSingleton;}

  CSettings();
  ~CSettings();

  SInputSettings &getInputSettings() {return m_InputSettings;}
  SVideoSettings &getVideoSettings() {return m_VideoSettings;}  

  void writeToFile();
private:

  void readFromFile();
};

#endif
