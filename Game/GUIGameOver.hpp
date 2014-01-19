#ifndef GUI_GAME_OVER
#define GUI_GAME_OVER

#include <CEGUI/CEGUI.h>
#include <OgreString.h>
#include <OgreSingleton.h>
#include "InputListener.hpp"

class CGUIGameOver : public Ogre::Singleton<CGUIGameOver>, public CInputListener {
private:
  CEGUI::Window *m_pRoot;
public:
  static CGUIGameOver &getSingleton();
  static CGUIGameOver *getSingletonPtr();


  CGUIGameOver(CEGUI::Window *pGUIRoot);
  ~CGUIGameOver();

  void update(Ogre::Real tpf);

  void show() {m_pRoot->setVisible(true); setInputListenerEnabled(true);}
  void hide() {m_pRoot->setVisible(false); setInputListenerEnabled(false);}

  virtual bool keyPressed( const OIS::KeyEvent &arg );
};

#endif // GUI_GAME_OVER
