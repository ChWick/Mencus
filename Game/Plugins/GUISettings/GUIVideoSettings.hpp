#ifndef _GUI_VIDEO_SETTINGS_HPP_
#define _GUI_VIDEO_SETTINGS_HPP_

#include "GUISettingsWindow.hpp"

class CGUIVideoSettings : public CGUISettingsWindow {
public:
  CGUIVideoSettings(CEGUI::Window *pParent);
protected:
  bool menuSizeSliderValueChanged(const CEGUI::EventArgs &args);
};

#endif
