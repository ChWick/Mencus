#ifndef _GUI_LEVEL_SELECT_HPP_
#define _GUI_LEVEL_SELECT_HPP_

#include "GUIOverlay.hpp"
#include <CEGUI/CEGUI.h>
#include "LevelList.hpp"

class CGUILevelSelect : public CGUIOverlay {
private:
  CEGUI::Window *m_pRoot;
  CEGUI::Window *m_pContent;
  CEGUI::Window *m_pChickenButton;

  // level selection windows
  CLevelList m_LevelList;
  unsigned int m_uiSelectedLevelID;
  SLevelInfo *m_pLevelInfo;
public:
  CGUILevelSelect(CEGUI::Window *pParent);
  virtual ~CGUILevelSelect();

protected:
  bool onBackButtonClicked(const CEGUI::EventArgs &args);
  bool onPlayButtonClicked(const CEGUI::EventArgs &args);


  // level selection
  void updateLevelsSelection();
  void selectLevel(unsigned int id);
  bool onLevelButtonClicked(const CEGUI::EventArgs &);
  bool onChickenPressed(const CEGUI::EventArgs&);
};

#endif
