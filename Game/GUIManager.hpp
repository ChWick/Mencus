#ifndef GUIMANAGER_HPP
#define GUIMANAGER_HPP

#include <OgreSingleton.h>
#include <OgrePrerequisites.h>
#include <OgreRenderQueueListener.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include "InputListener.hpp"
#include <vector>

class CGUIInput;

class CGUIManager :
  public CInputListener,
  public Ogre::Singleton<CGUIManager>,
  public Ogre::RenderQueueListener {

private:
  Ogre::SceneManager *m_pSceneManager;

  CEGUI::OgreRenderer* m_pCEGuiOgreRenderer;
  //! ImageCodec to use.  Set in subclass constructor, may be 0.
  CEGUI::ImageCodec* m_pCEGuiImageCodec;
  //! ResourceProvider to use.  Set in subclass constructor, may be 0.
  CEGUI::ResourceProvider* m_pCEGuiResourceProvider;

  CGUIInput *m_pGUIInput;

  bool m_bRenderPause;

  CEGUI::Sizef m_vNativeRes;
  std::vector<CEGUI::String> m_vFonts;
public:
  static CGUIManager& getSingleton(void);
  static CGUIManager* getSingletonPtr(void);

  CGUIManager(Ogre::SceneManager *pSceneManager, Ogre::RenderTarget &target);
  ~CGUIManager();

  void createResources();
  void destroyResources();
  void reloadResources();

  void update(Ogre::Real tpf);

  CEGUI::OgreRenderer *getRenderer() const {return m_pCEGuiOgreRenderer;}

  // Ogre::RenderQueueListener
  void renderQueueStarted(Ogre::uint8 id, const Ogre::String& invocation, bool& skipThisQueue);
  void renderQueueEnded(Ogre::uint8 id, const Ogre::String& invocation, bool& repeatThisQueue);

  // OIS::KeyListener
  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );

  // OIS::MouseListener
  virtual bool mouseMoved( const OIS::MouseEvent &arg );
  virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

  // OIS::MultitouchListener
  virtual bool touchMoved(const OIS::MultiTouchEvent& evt);
  virtual bool touchPressed(const OIS::MultiTouchEvent& evt);
  virtual bool touchReleased(const OIS::MultiTouchEvent& evt);
  virtual bool touchCancelled(const OIS::MultiTouchEvent& evt);

private:
  void createFreeTypeFont(const CEGUI::String &name, int size, const CEGUI::String &ttfFile);
private:
   Ogre::RenderQueueGroupID m_nRenderQueue; //!< When we draw the UI.
   bool m_bPostQueue;                       //!< Whether to draw CEGUI immediately before or after m_nRenderQueue
};

#endif
