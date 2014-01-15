#include "GUIManager.hpp"
#include "HUD.hpp"
#include "MainMenu.hpp"
#include "GUIInstructions.hpp"
#include <CEGUI/CEGUI.h>
#include <OgreSceneManager.h>

template<> CGUIManager* Ogre::Singleton<CGUIManager>::msSingleton = 0;

CGUIManager* CGUIManager::getSingletonPtr(void)
{
  return msSingleton;
}
CGUIManager& CGUIManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

CGUIManager::CGUIManager(Ogre::SceneManager *pSceneManager)
: m_pSceneManager(pSceneManager),
  m_nRenderQueue(Ogre::RENDER_QUEUE_OVERLAY),
  m_bPostQueue(false) {

  pSceneManager->addRenderQueueListener(this);

  CEGUI::Window *guiRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "MasterRoot");
  guiRoot->setAlpha(0);
  //guiRoot->setProperty("BackgroundEnabled", "false");
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(guiRoot);

  new CHUD(guiRoot);
  new CGUIInstructions(guiRoot);
  new CMainMenu(guiRoot);
}
CGUIManager::~CGUIManager() {
  m_pSceneManager->removeRenderQueueListener(this);
  delete CHUD::getSingletonPtr();
  delete CGUIInstructions::getSingletonPtr();
  delete CMainMenu::getSingletonPtr();
}
void CGUIManager::update(Ogre::Real tpf) {
  CHUD::getSingleton().update(tpf);
  CMainMenu::getSingleton().update(tpf);
}
void CGUIManager::renderQueueStarted(Ogre::uint8 id, const Ogre::String& invocation, bool& skipThisQueue) {
   // make sure you check the invocation string, or you can end up rendering the GUI multiple times
   // per frame due to shadow cameras.
   if ( !m_bPostQueue && m_nRenderQueue == id && invocation == "" )
   {
      CEGUI::System::getSingleton().renderAllGUIContexts();
   }
}

void CGUIManager::renderQueueEnded(Ogre::uint8 id, const Ogre::String& invocation, bool& repeatThisQueue) {
   if ( m_bPostQueue && m_nRenderQueue == id && invocation == "" )
   {
      CEGUI::System::getSingleton().renderAllGUIContexts();
   }
}
