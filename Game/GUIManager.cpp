/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#include "GUIManager.hpp"
#include "HUD.hpp"
#include "MainMenu.hpp"
#include "GUIInstructions.hpp"
#include "GUIGameOver.hpp"
#include <OgreSceneManager.h>
#include "GUIInput.hpp"
#include "InputDefines.hpp"
#include "GUIStatistics.hpp"
#include "GUITutorial.hpp"
#include "MapEditor.hpp"
#include "Game.hpp"
#include <SdkTrays.h>
#include "Settings.hpp"
#include "Plugins/SocialGaming/Overlay.hpp"
#include "Plugins/GUIToolsMenu.hpp"
#include <dependencies/OgreSdkUtil/SdkTrays.h>

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
  m_vNativeRes(target.getWidth(), target.getHeight()),
  m_MouseMoving(MD_COUNT, false),
  m_fMouseSpeedX(0),
  m_fMouseSpeedY(0),
  m_fMouseAcceleration(2),
  m_fMouseInitialSpeed(50) {
  CInputListenerManager::getSingleton().addInputListener(this);

  OgreBites::SdkTrayManager *pTrayMgr = CGame::getSingleton().showLoadingBar(1, 1);
  pTrayMgr->loadBarSetCaption("GUI ...");
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing CEGUI ***");

  Ogre::LogManager::getSingletonPtr()->logMessage("    creating bootstrapSystem");
  pTrayMgr->userUpdateLoadBar("Creating bootstrap system", 0.1);
  m_pCEGuiOgreRenderer = &CEGUI::OgreRenderer::bootstrapSystem(target);
  m_pCEGuiOgreRenderer->setFrameControlExecutionEnabled(false);

  Ogre::LogManager::getSingletonPtr()->logMessage("    setting up resource paths");
  pTrayMgr->userUpdateLoadBar("Setting up resource paths", 0.1);
  CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
  CEGUI::Font::setDefaultResourceGroup("Fonts");
  CEGUI::Scheme::setDefaultResourceGroup("Schemes");
  CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
  CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

  Ogre::LogManager::getSingletonPtr()->logMessage("    creating scheme");
  pTrayMgr->userUpdateLoadBar("Creating scheme", 0.1);
  createResources();

  pSceneManager->addRenderQueueListener(this);

  Ogre::LogManager::getSingletonPtr()->logMessage("    creating root window");
  pTrayMgr->userUpdateLoadBar("Initializing the root window", 0.1);
  CEGUI::Window *guiRoot = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "MasterRoot");
  //guiRoot->setAlpha(0);
  guiRoot->setSize(USize(UDim(1, 0), UDim(1, 0)));
  //guiRoot->setProperty("BackgroundEnabled", "false");
  CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(guiRoot);
#ifdef INPUT_MOUSE
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
#else
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseInvisible");
#endif
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setPosition(CEGUI::Vector2f(0,0));
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setConstraintArea(NULL);


  pTrayMgr->userUpdateLoadBar("Creating fonts", 0.2);
  createFreeTypeFont("dejavusans12", 12, "DejaVuSans.ttf");
  createFreeTypeFont("dejavusans8", 8, "DejaVuSans.ttf");
  createFreeTypeFont("diploma15", 15, "diploma.ttf");
  createFreeTypeFont("diploma20", 20, "diploma.ttf");
  createFreeTypeFont("dejavusans20", 20, "DejaVuSans.ttf");
  CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont( "dejavusans12" );
  guiRoot->setFont("dejavusans12");


  pTrayMgr->userUpdateLoadBar("Creating gui components", 0.2);
  //destroyResources();
  CGUIToolsMenu *pToolsMenu = new CGUIToolsMenu(guiRoot);
  new CGUIInstructions(guiRoot);
  new CGUIGameOver(guiRoot);
  new CGUIStatistics(guiRoot);
#ifdef INPUT_TOUCH
  m_pGUIInput = new CGUIInput(guiRoot); // gui input before hud, since hud depends
#endif
  new CHUD(guiRoot, pToolsMenu);
  Ogre::LogManager::getSingleton().logMessage("Singleton for map editor");
#if ENABLE_MAP_EDITOR
  new CMapEditor(guiRoot);
#endif
  new CGUITutorial(guiRoot);
  new CMainMenu(guiRoot);

  #if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
  m_lGUIOverlays.push_back(new SocialGaming::COverlay(guiRoot->getChild("MainMenuRoot")));
  m_lGUIOverlays.push_back(pToolsMenu);
  #endif

  pTrayMgr->userUpdateLoadBar("done...", 0.2);
  onGUIScalingChanged(CSettings::getSingleton().getVideoSettings().m_fHUDSize);
  Sizef vSize = CGUIManager::getSingleton().getNativeRes();
  float maxValue = std::min(vSize.d_height / 4.0, vSize.d_width / 8.0);
  changeTouchButtonSize(std::min(CSettings::getSingleton().getInputSettings().m_fTouchButtonSize, maxValue));
  Ogre::LogManager::getSingleton().logMessage("GUIManager initialized...");
  CGame::getSingleton().hideLoadingBar();
}
CGUIManager::~CGUIManager() {
  CInputListenerManager::getSingleton().removeInputListener(this);
  m_pSceneManager->removeRenderQueueListener(this);

  while (m_lGUIOverlays.size() > 0) {
    delete m_lGUIOverlays.front();
    m_lGUIOverlays.pop_front();
  }

  delete CHUD::getSingletonPtr();
  delete CGUIInstructions::getSingletonPtr();
  delete CGUIGameOver::getSingletonPtr();
  delete CMainMenu::getSingletonPtr();
  delete CGUIStatistics::getSingletonPtr();
  delete CGUITutorial::getSingletonPtr();
#ifdef INPUT_TOUCH
  delete m_pGUIInput;
#endif
#if ENABLE_MAP_EDITOR
  delete CMapEditor::getSingletonPtr();
#endif

  if (CEGUI::System::getSingletonPtr()) {CEGUI::OgreRenderer::destroySystem();}
}
void CGUIManager::update(Ogre::Real tpf) {
  CEGUI::System::getSingleton().injectTimePulse(tpf);

  // move mouse cursor
  if (CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().isVisible()) {
    m_fMouseSpeedX += m_fMouseSpeedX * m_fMouseAcceleration * tpf;
    m_fMouseSpeedY += m_fMouseSpeedY * m_fMouseAcceleration * tpf;
    CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(m_fMouseSpeedX * tpf, m_fMouseSpeedY * tpf);
  }
  
  if (m_bRenderPause) {return;}
  CHUD::getSingleton().update(tpf);
  CMainMenu::getSingleton().update(tpf);
#ifdef INPUT_TOUCH
  m_pGUIInput->update(tpf);
#endif
#if ENABLE_MAP_EDITOR
  CMapEditor::getSingleton().render();
#endif

  for (CGUIOverlay *pOverlay : m_lGUIOverlays) {
    pOverlay->update(tpf);
  }
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

  switch (arg.key) {
  case OIS::KC_LEFT:
    m_MouseMoving[MD_LEFT] = true;
    m_fMouseSpeedX = -m_fMouseInitialSpeed;
    break;
  case OIS::KC_RIGHT:
    m_MouseMoving[MD_RIGHT] = true;
    m_fMouseSpeedX = m_fMouseInitialSpeed;
    break;
  case OIS::KC_UP:
    m_MouseMoving[MD_UP] = true;
    m_fMouseSpeedY = -m_fMouseInitialSpeed;
    break;
  case OIS::KC_DOWN:
    m_MouseMoving[MD_DOWN] = true;
    m_fMouseSpeedY = m_fMouseInitialSpeed;
    break;
  case OIS::KC_RETURN:
    if (CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().isVisible()) {
      sys.getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
    }
    break;
  default:
    break;
  }

  return true;
}

bool CGUIManager::keyReleased( const OIS::KeyEvent &arg ) {
  CEGUI::System &sys = CEGUI::System::getSingleton();
  sys.getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(arg.key));

  switch (arg.key) {
  case OIS::KC_LEFT:
    m_MouseMoving[MD_LEFT] = false;
    m_fMouseSpeedX = 0;
    break;
  case OIS::KC_RIGHT:
    m_MouseMoving[MD_RIGHT] = false;
    m_fMouseSpeedX = 0;
    break;
  case OIS::KC_UP:
    m_MouseMoving[MD_UP] = false;
    m_fMouseSpeedY = 0;
    break;
  case OIS::KC_DOWN:
    m_MouseMoving[MD_DOWN] = false;
    m_fMouseSpeedY = 0;
    break;
  case OIS::KC_RETURN:
    if (CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().isVisible()) {
      sys.getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);
    }
    break;
  default:
    break;
  }

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
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  // Scroll wheel.
  if (arg.state.Z.rel)
    sys.getDefaultGUIContext().injectMouseWheelChange(arg.state.Z.rel / 120.0f);


  CEGUI::MouseEventArgs args(NULL);
  args.position = CEGUI::Vector2f(arg.state.X.abs, arg.state.Y.abs);
  args.moveDelta = CEGUI::Vector2f(arg.state.X.rel, arg.state.Y.rel);
  CEGUI::GlobalEventSet::getSingleton().fireEvent(CEGUI::Window::EventMouseMove, args);

  return true;
}
bool CGUIManager::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertButton(id));
  CEGUI::MouseEventArgs args(NULL);
  args.position = CEGUI::Vector2f(arg.state.X.abs, arg.state.Y.abs);
  args.moveDelta = CEGUI::Vector2f(arg.state.X.rel, arg.state.Y.rel);
  CEGUI::GlobalEventSet::getSingleton().fireEvent(CEGUI::Window::EventMouseButtonDown, args);

  return true;
}
bool CGUIManager::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertButton(id));
  CEGUI::MouseEventArgs args(NULL);
  args.position = CEGUI::Vector2f(arg.state.X.abs, arg.state.Y.abs);
  args.moveDelta = CEGUI::Vector2f(arg.state.X.rel, arg.state.Y.rel);
  CEGUI::GlobalEventSet::getSingleton().fireEvent(CEGUI::Window::EventMouseButtonUp, args);

  return true;
}
bool CGUIManager::touchMoved(const OIS::MultiTouchEvent& arg) {
  //sys.getDefaultGUIContext().injectMouseMove(evt.state.X.rel, evt.state.Y.rel);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  CEGUI::MouseEventArgs args(NULL);
  args.position = CEGUI::Vector2f(arg.state.X.abs, arg.state.Y.abs);
  args.moveDelta = CEGUI::Vector2f(arg.state.X.rel, arg.state.Y.rel);
  CEGUI::GlobalEventSet::getSingleton().fireEvent(CEGUI::Window::EventMouseMove, args);

  return true;
}
bool CGUIManager::touchPressed(const OIS::MultiTouchEvent& arg) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(CEGUI::LeftButton);
  CEGUI::MouseEventArgs args(NULL);
  args.position = CEGUI::Vector2f(arg.state.X.abs, arg.state.Y.abs);
  args.moveDelta = CEGUI::Vector2f(arg.state.X.rel, arg.state.Y.rel);
  CEGUI::GlobalEventSet::getSingleton().fireEvent(CEGUI::Window::EventMouseButtonDown, args);

  return true;
}
bool CGUIManager::touchReleased(const OIS::MultiTouchEvent& arg) {
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(CEGUI::LeftButton);
  CEGUI::MouseEventArgs args(NULL);
  args.position = CEGUI::Vector2f(arg.state.X.abs, arg.state.Y.abs);
  args.moveDelta = CEGUI::Vector2f(arg.state.X.rel, arg.state.Y.rel);
  CEGUI::GlobalEventSet::getSingleton().fireEvent(CEGUI::Window::EventMouseButtonUp, args);

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
  CEGUI::ImageManager::getSingleton().loadImageset("social_gaming_logos.imageset");
#if MENCUS_HAS_INPUT_BUTTONS_IMAGESET == 1
  CEGUI::ImageManager::getSingleton().loadImageset("input_buttons.imageset");
#endif

#ifdef INPUT_MOUSE
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseArrow");
#else
  CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("OgreTrayImages/MouseInvisible");
#endif
}
void CGUIManager::destroyResources() {
  m_bRenderPause = true;
  CEGUI::ImageManager::getSingleton().destroyImageCollection("main_menu_background");
  CEGUI::ImageManager::getSingleton().destroyImageCollection("hud_weapons");
  CEGUI::ImageManager::getSingleton().destroyImageCollection("save_pictures");
  CEGUI::SchemeManager::getSingleton().destroy("OgreTray");
  CEGUI::ImageManager::getSingleton().destroyImageCollection("OgreTrayImages");
  CEGUI::ImageManager::getSingleton().destroyImageCollection("social_gaming_logos");

#if MENCUS_HAS_INPUT_BUTTONS_IMAGESET == 1
  CEGUI::ImageManager::getSingleton().destroyImageCollection("input_buttons");
#endif
}
void CGUIManager::reloadResources() {
  m_pCEGuiOgreRenderer->getTexture("OgreTrayImages").loadFromFile("OgreTrayImages.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("main_menu_background").loadFromFile("main_menu_background.jpg", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("game_over").loadFromFile("game_over.jpg", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("hud_weapons").loadFromFile("hud_weapons.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("hud").loadFromFile("hud_main.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("save_pictures").loadFromFile("save_pictures.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("white").loadFromFile("white.png", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("instructions").loadFromFile("instr_scroll.jpg", "Imagesets");
  m_pCEGuiOgreRenderer->getTexture("social_gaming_logos").loadFromFile("social_gaming_logos.png", "Imagesets");

#if MENCUS_HAS_INPUT_BUTTONS_IMAGESET == 1
  m_pCEGuiOgreRenderer->getTexture("input_buttons").loadFromFile("input_buttons.png", "Imagesets");
#endif

  for (auto &sFontName : m_vFonts) {
    CEGUI::FontManager::getSingleton().get(sFontName).notifyDisplaySizeChanged(m_vNativeRes);
  }
#if ENABLE_MAP_EDITOR
  CMapEditor::getSingleton().reloadTextures();
#endif
}
void CGUIManager::resize(const CEGUI::Sizef &vSize) {
  m_vNativeRes = vSize;
  m_pCEGuiOgreRenderer->setDisplaySize(vSize);
#ifdef INPUT_TOUCH
  m_pGUIInput->windowResized();
#endif
  CMainMenu::getSingleton().windowSizeChanged(vSize);

  CEGUI::String smallfont("dejavusans8");
  CEGUI::String bigfont("dejavusans12");
  if (vSize.d_height > 800) {
    smallfont = "dejavusans20";
  }
  else if (vSize.d_height > 500) {
    smallfont = "dejavusans12";
  }

  if (vSize.d_height > 600) {
    bigfont = "dejavusans20";
  }

  CGUIStatistics::getSingleton().resize(smallfont, bigfont);
}
void CGUIManager::changeTouchButtonSize(float fSize) {
#ifdef INPUT_TOUCH
  m_pGUIInput->buttonSizeChanged(fSize);
#endif
  for (CGUIOverlay *pOverlay : m_lGUIOverlays) {
    pOverlay->changeTouchButtonSize(fSize);
  }
}

void CGUIManager::onGUIScalingChanged(float fScaling) {
  for (CGUIOverlay *pOverlay : m_lGUIOverlays) {
    pOverlay->onGUIScalingChanged(fScaling);
  }
  CMainMenu::getSingleton().resizeGUI(fScaling);
}
