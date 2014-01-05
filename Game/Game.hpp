#ifndef _GAME_H_
#define _GAME_H_

#include <Ogre.h>
#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "InputListener.hpp"
#include "dependencies/OgreSdkUtil/SdkCameraMan.h"
#include "dependencies/OgreSdkUtil/SdkTrays.h"

class CGameState;

class CGame : public CInputListener, public Ogre::FrameListener, public Ogre::WindowEventListener, OgreBites::SdkTrayListener, public Ogre::Singleton<CGame> {
private:
  CGameState *m_pGameState;

  CEGUI::Renderer* m_pCEGuiOgreRenderer;
  //! ImageCodec to use.  Set in subclass constructor, may be 0.
  CEGUI::ImageCodec* m_pCEGuiImageCodec;
  //! ResourceProvider to use.  Set in subclass constructor, may be 0.
  CEGUI::ResourceProvider* m_pCEGuiResourceProvider;
public:
  CGame(void);
  virtual ~CGame(void);

  static CGame &getSingleton();
  static CGame *getSingletonPtr();
  bool go(void);

  OIS::Keyboard* getKeyboard() {return mKeyboard;}
  OIS::Mouse *getMouse() {return mMouse;}
  OgreBites::ParamsPanel* getDetailsPanel() {return mDetailsPanel;}

  void shutDown() { mShutDown = true; }
protected:
  Ogre::Root *mRoot;
  Ogre::Camera* mCamera;
  Ogre::SceneManager* mSceneMgr;
  Ogre::RenderWindow* mWindow;
  Ogre::String mResourcesCfg;
  Ogre::String mPluginsCfg;

  // OgreBites
  OgreBites::SdkTrayManager* mTrayMgr;
  OgreBites::SdkCameraMan* mCameraMan;      // basic camera controller
  OgreBites::ParamsPanel* mDetailsPanel;    // sample details panel
  bool mCursorWasVisible;                   // was cursor visible before dialog appeared
  bool mShutDown;

  // OIS Input devices
  OIS::InputManager* mInputManager;
  OIS::Keyboard* mKeyboard;
  OIS::Mouse* mMouse;

  // Ogre::FrameListener
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
  virtual bool frameStarted(const Ogre::FrameEvent& evt);
  virtual bool frameEnded(const Ogre::FrameEvent& evt);

  // OIS::KeyListener
  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  // Ogre::WindowEventListener
  virtual void windowResized(Ogre::RenderWindow* rw);
  virtual void windowClosed(Ogre::RenderWindow* rw);
};

#endif // #ifndef _GAME_H_
