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

  const SInputSettings &getInputSettings() const {return m_InputSettings;}
  const SVideoSettings &getVideoSettings() const {return m_VideoSettings;}
  
private:

  void readFromFile();
  void writeToFile();
};

#endif
