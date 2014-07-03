#ifndef _GUI_INPUT_SETTINGS_HPP_
#define _GUI_INPUT_SETTINGS_HPP_

#include "GUISettingsWindow.hpp"

class CGUIInputSettings : public CGUISettingsWindow {
public:
  CGUIInputSettings(CEGUI::Window *pParent);
protected:
  bool buttonSizeSliderValueChanged(const CEGUI::EventArgs &args);
};

#endif
