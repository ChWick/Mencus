#include "GUIManager.hpp"
#include "HUD.hpp"
#include <CEGUI/CEGUI.h>

template<> CGUIManager* Ogre::Singleton<CGUIManager>::msSingleton = 0;

CGUIManager* CGUIManager::getSingletonPtr(void)
{
  return msSingleton;
}
CGUIManager& CGUIManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

CGUIManager::CGUIManager() {
  CEGUI::Window *guiRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "MasterRoot");
  guiRoot->setAlpha(0);
  //guiRoot->setProperty("BackgroundEnabled", "false");
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(guiRoot);

  new CHUD(guiRoot);
}
CGUIManager::~CGUIManager() {
  delete CHUD::getSingletonPtr();
}
void CGUIManager::update(Ogre::Real tpf) {
  CHUD::getSingleton().update(tpf);
}
