#ifndef GUIMANAGER_HPP
#define GUIMANAGER_HPP

#include <OgreSingleton.h>
#include <OgrePrerequisites.h>
#include <OgreRenderQueueListener.h>

class CGUIManager :

  public Ogre::Singleton<CGUIManager>,
  public Ogre::RenderQueueListener {

private:
    Ogre::SceneManager *m_pSceneManager;

public:
  static CGUIManager& getSingleton(void);
  static CGUIManager* getSingletonPtr(void);

  CGUIManager(Ogre::SceneManager *pSceneManager);
  ~CGUIManager();

  void update(Ogre::Real tpf);

  // Ogre::RenderQueueListener
  void renderQueueStarted(Ogre::uint8 id, const Ogre::String& invocation, bool& skipThisQueue);
  void renderQueueEnded(Ogre::uint8 id, const Ogre::String& invocation, bool& repeatThisQueue);

private:
   Ogre::RenderQueueGroupID m_nRenderQueue; //!< When we draw the UI.
   bool m_bPostQueue;                       //!< Whether to draw CEGUI immediately before or after m_nRenderQueue
};

#endif
