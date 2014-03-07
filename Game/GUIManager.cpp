#include "GUIManager.hpp"
#include "HUD.hpp"
#include "MainMenu.hpp"
#include "GUIInstructions.hpp"
#include "GUIGameOver.hpp"
#include <OgreSceneManager.h>
#include "GUIInput.hpp"

using namespace CEGUI;

template<> CGUIManager* Ogre::Singleton<CGUIManager>::msSingleton = 0;

CGUIManager* CGUIManager::getSingletonPtr(void)
{
  return msSingleton;
}
CGUIManager& CGUIManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

CGUIManager::CGUIManager(Ogre::SceneManager *pSceneManager, Ogre::RenderTarget &target)
: m_pSceneManager(pSceneManager),
  m_nRenderQueue(Ogre::RENDER_QUEUE_OVERLAY),
  m_bPostQueue(false) {
  CInputListenerManager::getSingleton().addInputListener(this);

  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing CEGUI ***");
  Ogre::LogManager::getSingletonPtr()->logMessage("    creating bootstrapSystem");
  m_pCEGuiOgreRenderer = &CEGUI::OgreRenderer::bootstrapSystem(target);
  m_pCEGuiOgreRenderer->setFrameControlExecutionEnabled(false);
  Ogre::LogManager::getSingletonPtr()->logMessage("    setting upresource paths");
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

  Ogre::LogManager::getSingletonPtr()->logMessage("    creating scheme");
  CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");

  pSceneManager->addRenderQueueListener(this);

  Ogre::LogManager::getSingletonPtr()->logMessage("    creating root window");
  CEGUI::Window *guiRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "MasterRoot");
  //guiRoot->setAlpha(0);
  guiRoot->setSize(USize(UDim(1, 0), UDim(1, 0)));
  //guiRoot->setProperty("BackgroundEnabled", "false");
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(guiRoot);
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(CEGUI::Vector2f(0,0));

  CEGUI::FontManager::getSingleton().createFreeTypeFont("dejavusans12", 12, true, "DejaVuSans.ttf" );
  CEGUI::FontManager::getSingleton().createFreeTypeFont("dejavusans8", 8, true, "DejaVuSans.ttf" );
  CEGUI::FontManager::getSingleton().createFreeTypeFont("diploma15", 15, true, "diploma.ttf" );
  CEGUI::FontManager::getSingleton().createFreeTypeFont("diploma20", 20, true, "diploma.ttf" );
  CEGUI::FontManager::getSingleton().createFreeTypeFont("dejavusans20", 20, true, "DejaVuSans.ttf" );
  guiRoot->setFont("dejavusans12");

  new CHUD(guiRoot);
  new CGUIInstructions(guiRoot);
  new CGUIGameOver(guiRoot);
  new CMainMenu(guiRoot);
  m_pGUIInput = new CGUIInput(guiRoot);
}
CGUIManager::~CGUIManager() {
  CInputListenerManager::getSingleton().removeInputListener(this);
  m_pSceneManager->removeRenderQueueListener(this);
  delete CHUD::getSingletonPtr();
  delete CGUIInstructions::getSingletonPtr();
  delete CGUIGameOver::getSingletonPtr();
  delete CMainMenu::getSingletonPtr();
  delete m_pGUIInput;
  
  if (CEGUI::System::getSingletonPtr()) {CEGUI::OgreRenderer::destroySystem();}
}
void CGUIManager::update(Ogre::Real tpf) {
  CEGUI::System::getSingleton().injectTimePulse(tpf);
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

//-------------------------------------------------------------------------------------
bool CGUIManager::keyPressed( const OIS::KeyEvent &arg ) {
  CEGUI::System &sys = CEGUI::System::getSingleton();
  sys.getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(arg.key));
  sys.getDefaultGUIContext().injectChar(arg.text);

  return true;
}

bool CGUIManager::keyReleased( const OIS::KeyEvent &arg ) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(arg.key));

  return true;
}
CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID) {
  switch (buttonID) {
  case OIS::MB_Left:
    return CEGUI::LeftButton;

  case OIS::MB_Right:
    return CEGUI::RightButton;

  case OIS::MB_Middle:
    return CEGUI::MiddleButton;

  default:
    return CEGUI::LeftButton;
  }
}
bool CGUIManager::mouseMoved( const OIS::MouseEvent &arg ) {
  CEGUI::System &sys = CEGUI::System::getSingleton();
  sys.getDefaultGUIContext().injectMouseMove(arg.state.X.rel, arg.state.Y.rel);
  // Scroll wheel.
  if (arg.state.Z.rel)
    sys.getDefaultGUIContext().injectMouseWheelChange(arg.state.Z.rel / 120.0f);

  return true;
}
bool CGUIManager::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertButton(id));

  return true;
}
bool CGUIManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertButton(id));

  return true;
}
bool CGUIManager::touchMoved(const OIS::MultiTouchEvent& evt) {
  CEGUI::System &sys = CEGUI::System::getSingleton();
  sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);

  return true;
}
bool CGUIManager::touchPressed(const OIS::MultiTouchEvent& evt) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(evt.state.X.abs, evt.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);

  return true;
}
bool CGUIManager::touchReleased(const OIS::MultiTouchEvent& evt) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(evt.state.X.abs, evt.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);

  return true;
}
bool CGUIManager::touchCancelled(const OIS::MultiTouchEvent& evt) {
  return true;
}
