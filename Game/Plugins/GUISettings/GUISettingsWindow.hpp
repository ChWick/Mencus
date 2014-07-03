#ifndef _GUI_SETTINGS_WINDOW_HPP_
#define _GUI_SETTINGS_WINDOW_HPP_

#include <CEGUI/CEGUI.h>
#include "Plugins/GUIOverlay.hpp"

class CGUISettingsWindow : public CGUIOverlay {
protected:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pContent;
public:
  CGUISettingsWindow(CEGUI::Window *pParentRoot, const CEGUI::String &sTitle);
  virtual ~CGUISettingsWindow();

protected:
  bool onCloseButtonClicked(const CEGUI::EventArgs &args);
};

#endif
