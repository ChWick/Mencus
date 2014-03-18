#ifndef _GUI_TUTORIAL_HPP_
#define _GUI_TUTORIAL_HPP_

#include <CEGUI/CEGUI.h>
#include <OgreSingleton.h>
#include "PauseCaller.hpp"

class CGUITutorial 
  : public Ogre::Singleton<CGUITutorial>,
    public CPauseCaller {
  CEGUI::Window *m_pRoot;
public:
  static CGUITutorial &getSingleton();
  static CGUITutorial *getSingletonPtr();

  CGUITutorial(CEGUI::Window *pRoot);

  void showMoveInformationWindow();
  void showSwitchInformationWindow();
  void showEnemyInformationWindow();
  void showSmallEnemyInformationWindow();

private:
  void showSimpleTextMessageBox(const CEGUI::String &label, const CEGUI::String &content);
  CEGUI::Window *createMessageBox(const CEGUI::String &label);

  bool onCloseButtonClicked(const CEGUI::EventArgs &);
};


#endif
