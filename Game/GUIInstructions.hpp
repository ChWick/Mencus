#ifndef INSTRUCTIONS_GUI_HPP
#define INSTRUCTIONS_GUI_HPP

#include <CEGUI/CEGUI.h>
#include <OgreString.h>
#include <OgreSingleton.h>

class CGUIInstructions : public Ogre::Singleton<CGUIInstructions> {
private:
  CEGUI::Window *m_pInstructionsRoot;
  CEGUI::Window *m_pText;
public:
  static CGUIInstructions &getSingleton();
  static CGUIInstructions *getSingletonPtr();


  CGUIInstructions(CEGUI::Window *pGUIRoot);

  void setText(const CEGUI::String &sText) {m_pText->setText(sText);}

  void show() {m_pInstructionsRoot->setVisible(true);}
  void hide() {m_pInstructionsRoot->setVisible(false);}
};
#endif // INSTRUCTIONS_GUI_HPP
