#ifndef INSTRUCTIONS_GUI_HPP
#define INSTRUCTIONS_GUI_HPP

#include <CEGUI/CEGUI.h>
#include <OgreString.h>
#include <OgreSingleton.h>
#include "InputListener.hpp"
#include "ScreenplayListener.hpp"

class CGUIInstructions : public Ogre::Singleton<CGUIInstructions>, public CInputListener {
private:
  CEGUI::Window *m_pInstructionsRoot;
  CEGUI::Window *m_pText;
  CEGUI::Window *m_pPressKeyText;
  CScreenplayListener *m_pScreenplayListener;
public:
  static CGUIInstructions &getSingleton();
  static CGUIInstructions *getSingletonPtr();


  CGUIInstructions(CEGUI::Window *pGUIRoot);
  ~CGUIInstructions();

  void setText(const CEGUI::String &sText) {m_pText->setText(sText);}

  void show() {m_pInstructionsRoot->setVisible(true); setInputListenerEnabled(true);}
  void hide() {m_pInstructionsRoot->setVisible(false); setInputListenerEnabled(false);}

  void setScreenplayListener(CScreenplayListener *pListener) {m_pScreenplayListener = pListener;}

  virtual bool keyPressed( const OIS::KeyEvent &arg );
};
#endif // INSTRUCTIONS_GUI_HPP
