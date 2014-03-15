#include "GUIManager.hpp"
#include "HUD.hpp"
#include "MainMenu.hpp"
#include "GUIInstructions.hpp"
#include "GUIGameOver.hpp"
#include <OgreSceneManager.h>
#include "GUIInput.hpp"
#include "InputDefines.hpp"

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
  m_pGUIInput(NULL),
  m_nRenderQueue(Ogre::RENDER_QUEUE_OVERLAY),
  m_bPostQueue(false),
  m_bRenderPause(false),
  m_vNativeRes(target.getWidth(), target.getHeight()) {
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
  createResources();

  pSceneManager->addRenderQueueListener(this);

  Ogre::LogManager::getSingletonPtr()->logMessage("    creating root window");
  CEGUI::Window *guiRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "MasterRoot");
  //guiRoot->setAlpha(0);
  guiRoot->setSize(USize(UDim(1, 0), UDim(1, 0)));
  //guiRoot->setProperty("BackgroundEnabled", "false");
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(guiRoot);
#ifdef INPUT_MOUSE
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(CEGUI::Vector2f(0,0));
#else
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setVisible(false);
#endif

  createFreeTypeFont("dejavusans12", 12, "DejaVuSans.ttf");
  createFreeTypeFont("dejavusans8", 8, "DejaVuSans.ttf");
  createFreeTypeFont("diploma15", 15, "diploma.ttf");
  createFreeTypeFont("diploma20", 20, "diploma.ttf");
  createFreeTypeFont("dejavusans20", 20, "DejaVuSans.ttf");
  guiRoot->setFont("dejavusans12");


  //destroyResources();

#ifdef INPUT_TOUCH
  m_pGUIInput = new CGUIInput(guiRoot); // gui input before hud, since hud depends
#endif
  new CHUD(guiRoot, m_pGUIInput);
  new CGUIInstructions(guiRoot);
  new CGUIGameOver(guiRoot);
  new CMainMenu(guiRoot);

}
CGUIManager::~CGUIManager() {
  CInputListenerManager::getSingleton().removeInputListener(this);
  m_pSceneManager->removeRenderQueueListener(this);
  delete CHUD::getSingletonPtr();
  delete CGUIInstructions::getSingletonPtr();
  delete CGUIGameOver::getSingletonPtr();
  delete CMainMenu::getSingletonPtr();
#ifdef INPUT_TOUCH
  delete m_pGUIInput;
#endif
  
  if (CEGUI::System::getSingletonPtr()) {CEGUI::OgreRenderer::destroySystem();}
}
void CGUIManager::update(Ogre::Real tpf) {
  CEGUI::System::getSingleton().injectTimePulse(tpf);
  if (m_bRenderPause) {return;}
  CHUD::getSingleton().update(tpf);
  CMainMenu::getSingleton().update(tpf);
#ifdef INPUT_TOUCH
  m_pGUIInput->update(tpf);
#endif
}
void CGUIManager::renderQueueStarted(Ogre::uint8 id, const Ogre::String& invocation, bool& skipThisQueue) {
   // make sure you check the invocation string, or you can end up rendering the GUI multiple times
   // per frame due to shadow cameras.
   if ( !m_bPostQueue && m_nRenderQueue == id && invocation == "" && !m_bRenderPause)
   {
     CEGUI::System::getSingleton().renderAllGUIContexts();
   }
}

void CGUIManager::renderQueueEnded(Ogre::uint8 id, const Ogre::String& invocation, bool& repeatThisQueue) {
   if ( m_bPostQueue && m_nRenderQueue == id && invocation == "" && !m_bRenderPause)
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
  //sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(evt.state.X.abs, evt.state.Y.abs);

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

void CGUIManager::createFreeTypeFont(const CEGUI::String &name, int size, const CEGUI::String &ttfFile) {
  CEGUI::FontManager::getSingleton().createFreeTypeFont(name, size, true, ttfFile, "Fonts", CEGUI::ASM_Both, m_vNativeRes);
  m_vFonts.push_back(name);  
}
void CGUIManager::createResources() {
  m_bRenderPause = false;
  CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
  
  CEGUI::ImageManager::getSingleton().loadImageset("main_menu_background.imageset");
  CEGUI::ImageManager::getSingleton().loadImageset("hud_weapons.imageset");
  CEGUI::ImageManager::getSingleton().loadImageset("save_pictures.imageset");
  CEGUI::ImageManager::getSingleton().loadImageset("hud.imageset");
  CEGUI::ImageManager::getSingleton().loadImageset("game_over.imageset");
  CEGUI::ImageManager::getSingleton().loadImageset("white.imageset");
#ifdef INPUT_KEYBOARD
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
#endif
}
void CGUIManager::destroyResources() {
  m_bRenderPause = true;
  CEGUI::ImageManager::getSingleton().destroyImageCollection("main_menu_background");
  CEGUI::ImageManager::getSingleton().destroyImageCollection("hud_weapons");
  CEGUI::ImageManager::getSingleton().destroyImageCollection("save_pictures");
  CEGUI::SchemeManager::getSingleton().destroy("OgreTray");
  CEGUI::ImageManager::getSingleton().destroyImageCollection("OgreTrayImages");
}
void CGUIManager::reloadResources() {
  m_pCEGuiOgreRenderer->getTexture("OgreTrayImages").loadFromFile("OgreTrayImages.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("main_menu_background").loadFromFile("main_menu_background.jpg", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("game_over").loadFromFile("game_over.jpg", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("hud_weapons").loadFromFile("hud_weapons.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("hud").loadFromFile("hud_main.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("save_pictures").loadFromFile("save_pictures.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("white").loadFromFile("white.png", "Imagesets");

  for (auto &sFontName : m_vFonts) {
    CEGUI::FontManager::getSingleton().get(sFontName).notifyDisplaySizeChanged(m_vNativeRes);
  }
}
void CGUIManager::resize(const CEGUI::Sizef &vSize) {
  m_vNativeRes = vSize;
  m_pCEGuiOgreRenderer->setDisplaySize(vSize);
#ifdef INPUT_TOUCH
  m_pGUIInput->windowResized();
#endif
  CMainMenu::getSingleton().windowSizeChanged(vSize);
}
void CGUIManager::changeTouchButtonSize(float fSize) {
#ifdef INPUT_TOUCH
    m_pGUIInput->buttonSizeChanged(fSize);
#endif
}
