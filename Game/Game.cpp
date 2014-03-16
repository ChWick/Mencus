#include "Game.hpp"
#include <OgreCodec.h>
#include "PauseManager.hpp"
#include "GUIManager.hpp"
#include "GameState.hpp"
#include "SaveStateManager.hpp"
#include "DebugDefines.hpp"
#include "ShaderManager.hpp"
#include "GameInputManager.hpp"
#include "SnapshotManager.hpp"
#include "InputDefines.hpp"

using namespace Ogre;

template<> CGame *Ogre::Singleton<CGame>::msSingleton = 0;

#ifdef _DEBUG
#define USE_DEBUG_PLUGINS
#endif


CGame *CGame::getSingletonPtr() {
  return msSingleton;
}
CGame &CGame::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

//-------------------------------------------------------------------------------------
CGame::CGame(void)
  :
  //m_pGameState(NULL),
    mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mTrayMgr(0),
    mCameraMan(0),
    mDetailsPanel(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(NULL) {
  mFSLayer = OGRE_NEW_T(Ogre::FileSystemLayer, Ogre::MEMCATEGORY_GENERAL)(OGRE_VERSION_NAME);
#ifdef INCLUDE_RTSHADER_SYSTEM
  mShaderGenerator	 = NULL;		
  mMaterialMgrListener   = NULL;
#endif // INCLUDE_RTSHADER_SYSTEM
}
//-------------------------------------------------------------------------------------
CGame::~CGame(void) {
  if (mTrayMgr) delete mTrayMgr;
  if (mCameraMan) delete mCameraMan;
  if (CGameState::getSingletonPtr()) { delete CGameState::getSingletonPtr(); }
  if (CGUIManager::getSingletonPtr()) {delete CGUIManager::getSingletonPtr();}
  if (CPauseManager::getSingletonPtr()) {delete CPauseManager::getSingletonPtr();}
  if (CGameInputManager::getSingletonPtr()) {delete CGameInputManager::getSingletonPtr();}
  if (CInputListenerManager::getSingletonPtr()) {delete CInputListenerManager::getSingletonPtr();}
  if (CSaveStateManager::getSingletonPtr()) {delete CSaveStateManager::getSingletonPtr();}

  if (CShaderManager::getSingletonPtr()) {delete CShaderManager::getSingletonPtr();}

  OGRE_DELETE_T(mFSLayer, FileSystemLayer, Ogre::MEMCATEGORY_GENERAL);
  //Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
  if (mRoot) {
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
    if (mOverlaySystem) {OGRE_DELETE mOverlaySystem;}
#endif
    OGRE_DELETE mRoot;
  }
}
void CGame::go() {
  initApp();
  mRoot->startRendering();
}
void CGame::initApp() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  createRoot();

  setup();

  //mRoot->saveConfig();

  Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();

  // Clear event times
  Ogre::Root::getSingleton().clearEventTimes();

#else
  createRoot();

  if (!oneTimeConfig());

  setup();

#endif
}
bool CGame::oneTimeConfig() {
  if (!mRoot->restoreConfig()) {
    return mRoot->showConfigDialog();
  }
  return true;
}
void CGame::closeApp() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    shutdown();
#else
    mRoot->saveConfig();
    shutdown();
    if (mRoot)
      {
    OGRE_DELETE mOverlaySystem;
    OGRE_DELETE mRoot;
  }
#ifdef OGRE_STATIC_LIB
    mStaticPluginLoader.unload();
#endif
#endif
}
void CGame::setup() {
  Ogre::LogManager::getSingletonPtr()->logMessage("*** creating window ***");
  mWindow = createWindow();
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Setting up input ***");
  setupInput(true);
  Ogre::LogManager::getSingletonPtr()->logMessage("*** locating resources ***");
  locateResources();
  Ogre::LogManager::getSingletonPtr()->logMessage("*** loading essential resources ***");
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Essential");
            
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
            
  // adds context as listener to process context-level (above the sample level) events
  mRoot->addFrameListener(this);
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
#endif

  Ogre::LogManager::getSingletonPtr()->logMessage("*** creating scene ***");
  createScene();
  Ogre::LogManager::getSingletonPtr()->logMessage("*** setup finished ***");
}
Ogre::RenderWindow *CGame::createWindow() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  return mWindow;
#else
  return mRoot->initialise(true);
#endif
}
void CGame::createRoot() {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  mRoot = Ogre::Root::getSingletonPtr();
#else
  Ogre::String pluginsPath = Ogre::StringUtil::BLANK;
#   ifndef OGRE_STATIC_LIB
  pluginsPath = mFSLayer->getConfigFilePath("plugins.cfg");
#   endif
  mRoot = OGRE_NEW Ogre::Root(pluginsPath, mFSLayer->getWritablePath("ogre.cfg"), 
			      mFSLayer->getWritablePath("ogre.log"));
            
#   ifdef OGRE_STATIC_LIB
  mStaticPluginLoader.load();
#   endif
#endif
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Finished root ***");
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Creating OverlaySystem ***");
  mOverlaySystem = OGRE_NEW Ogre::OverlaySystem();
  Ogre::LogManager::getSingletonPtr()->logMessage("*** Finished OverlaySystem***");
}
void CGame::locateResources() {
  // load resource paths from config file
  Ogre::ConfigFile cf;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  cf.load(openAPKFile(mFSLayer->getConfigFilePath("resources.cfg")));
#else
  cf.load(mFSLayer->getConfigFilePath("resources.cfg"));
#endif
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
  Ogre::String sec, type, arch;

  // go through all specified resource groups
  while (seci.hasMoreElements()) {
    sec = seci.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator i;

    // go through all resource paths
    for (i = settings->begin(); i != settings->end(); i++) {
      type = i->first;
      arch = i->second;
    
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch, type, sec);
    }
  }
}
void CGame::loadResources() {
  //Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  showLoadingBar();
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Imagesets");
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Fonts");
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Schemes");
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("LookNFeel");
  Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Layouts");
  hideLoadingBar();
}
void CGame::setupInput(bool nograb) {
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
  OIS::ParamList pl;
  size_t winHandle = 0;
  std::ostringstream winHandleStr;

  mWindow->getCustomAttribute("WINDOW", &winHandle);
  winHandleStr << winHandle;

  pl.insert(std::make_pair("WINDOW", winHandleStr.str()));
  if (nograb) {
    pl.insert(std::make_pair("x11_keyboard_grab", "false"));
    pl.insert(std::make_pair("x11_mouse_grab", "false"));
    pl.insert(std::make_pair("w32_mouse", "DISCL_FOREGROUND"));
    pl.insert(std::make_pair("w32_mouse", "DISCL_NONEXCLUSIVE"));
    pl.insert(std::make_pair("w32_keyboard", "DISCL_FOREGROUND"));
    pl.insert(std::make_pair("w32_keyboard", "DISCL_NONEXCLUSIVE"));
  }

  mInputManager = OIS::InputManager::createInputSystem(pl);

  createInputDevices();      // create the specific input devices
#endif

  // attach input devices
  windowResized(mWindow);    // do an initial adjustment of mouse area
  CInputListenerManager *pInputManager = new CInputListenerManager(mWindow);
  new CGameInputManager();
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
  
  if(mInputContext.mKeyboard)
    mInputContext.mKeyboard->setEventCallback(pInputManager);
  if(mInputContext.mMouse)
    mInputContext.mMouse->setEventCallback(pInputManager);

#else
  if(mInputContext.mMultiTouch)
    mInputContext.mMultiTouch->setEventCallback(pInputManager);
#endif

  pInputManager->addInputListener(this);
}
void CGame::createInputDevices() {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  mInputContext.mMultiTouch = static_cast<OIS::MultiTouch*>(mInputMgr->createInputObject(OIS::OISMultiTouch, true));
  mInputContext.mAccelerometer = static_cast<OIS::JoyStick*>(mInputMgr->createInputObject(OIS::OISJoyStick, true));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  // nothing to do
#elif OGRE_PLATFORM == OGRE_PLATFORM_WINRT
  // mInputManager is NULL and input devices are already passed to us, therefore nothing to do
  assert(mInputContext.mKeyboard);
  assert(mInputContext.mMouse);
#else
  mInputContext.mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
  mInputContext.mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
#endif
}
void CGame::shutdown() {
#if ENABLE_SHADERS_CACHE_SAVE == 1
  if (Ogre::GpuProgramManager::getSingleton().isCacheDirty()) {
    Ogre::String path = mFSLayer->getWritablePath(getShaderCacheFileName());
    FILE * outFile = fopen(path.c_str(), "wb");
    if (outFile) {
      Ogre::LogManager::getSingleton().logMessage("Writing shader cache to ");
      Ogre::LogManager::getSingleton().logMessage(path.c_str());
      Ogre::DataStreamPtr ostream(new Ogre::FileHandleDataStream(path.c_str(), outFile, Ogre::DataStream::WRITE));
      Ogre::GpuProgramManager::getSingleton().saveMicrocodeCache(ostream);
      ostream->close();
    }
  }
#endif

  if (mTrayMgr) {
    delete mTrayMgr;
    mTrayMgr = 0;
  }

  if (mRoot->getRenderSystem() != NULL) destroyScene();

    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  shutdownInput();

#ifdef INCLUDE_RTSHADER_SYSTEM
  // Destroy the RT Shader System.
  destroyRTShaderSystem();
#endif // INCLUDE_RTSHADER_SYSTEM
}
void CGame::shutdownInput() {
  // detach input devices
  windowResized(mWindow);    // do an initial adjustment of mouse area
  if(mInputContext.mKeyboard)
    mInputContext.mKeyboard->setEventCallback(NULL);
  if(mInputContext.mMouse)
    mInputContext.mMouse->setEventCallback(NULL);
#if OIS_WITH_MULTITOUCH
  if(mInputContext.mMultiTouch)
    mInputContext.mMultiTouch->setEventCallback(NULL);
#endif
  if(mInputContext.mAccelerometer)
    mInputContext.mAccelerometer->setEventCallback(NULL);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
  if (mInputManager) {
    if(mInputContext.mKeyboard)
      mInputManager->destroyInputObject(mInputContext.mKeyboard);
    if(mInputContext.mMouse)
      mInputManager->destroyInputObject(mInputContext.mMouse);
#if OIS_WITH_MULTITOUCH
    if(mInputContext.mMultiTouch)
      mInputManager->destroyInputObject(mInputContext.mMultiTouch);
#endif
    if(mInputContext.mAccelerometer)
      mInputManager->destroyInputObject(mInputContext.mAccelerometer);

    OIS::InputManager::destroyInputSystem(mInputManager);
    mInputManager = 0;
  }
#endif
}
void CGame::createScene() {
  assert(mRoot);
  assert(!mSceneMgr);
  assert(mWindow);
  if (!mWindow) {
    Ogre::LogManager::getSingletonPtr()->logMessage(Ogre::LML_CRITICAL, "mWindow is not set!");
  }

  //-------------------------------------------------------------------------------------
  // choose scenemanager
  // Get the SceneManager, in this case a generic one
  //	mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE, "MainSceneManager");
  Ogre::LogManager::getSingletonPtr()->logMessage("   create scene manager");
  mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "MainSceneManager");
  mSceneMgr->addRenderQueueListener(mOverlaySystem);
  //-------------------------------------------------------------------------------------
  // create camera
  // Create the camera
  Ogre::LogManager::getSingletonPtr()->logMessage("   create camera");
  mCamera = mSceneMgr->createCamera("GameCam");

  // Position it at 500 in Z direction
  //mCamera->setPosition(Ogre::Vector3(0,0,10));
  // Look back along -Z
  //mCamera->lookAt(Ogre::Vector3(0,0,0));
  //mCamera->setNearClipDistance(0.01f);
  //mCamera->setFarClipDistance(400.0f);

  
  Ogre::LogManager::getSingletonPtr()->logMessage("   create viewport");
  //mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controlle
  //mCameraMan->setTopSpeed(10);
  //------------------------------------------------------------------------------------
  // create viewports
  // Create one viewport, entire window
  Ogre::Viewport* vp = mWindow->addViewport(mCamera);
  vp->setBackgroundColour(Ogre::ColourValue(0,0,1));
  vp->setClearEveryFrame(true);
 
  // Alter the camera aspect ratio to match the viewport
  mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
  //-------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------
    // Create the scene
    //mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

 

  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing SdkTrays ***");
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
  mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, mInputContext, this);
#else
  mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, NULL, this);
#endif

#ifdef DEBUG_SHOW_OGRE_TRAY
  mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
  //mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);

  // create a params panel for displaying sample details
  Ogre::StringVector items;
  items.push_back("cam.x");
  items.push_back("cam.y");
  items.push_back("player.x");
  items.push_back("player.y");
  items.push_back("");
  items.push_back("");
  items.push_back("");
  items.push_back("");
  items.push_back("");
  items.push_back("Filtering");
  items.push_back("Poly Mode");

  mDetailsPanel = mTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 200, items);
  mDetailsPanel->setParamValue(9, "Bilinear");
  mDetailsPanel->setParamValue(10, "Solid");
  mDetailsPanel->hide();
#else
  mTrayMgr->hideCursor();
#endif // DEBUG_SHOW_OGRE_TRAY

#ifdef INPUT_MOUSE
#else
  mTrayMgr->hideCursor();
#endif  

  mRoot->addFrameListener(this);


#ifdef INCLUDE_RTSHADER_SYSTEM
  // Initialize shader generator.
  // Must be before resource loading in order to allow parsing extended material attributes.
  bool success = initialiseRTShaderSystem(mSceneMgr);
  if (!success) {
    OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, 
		"Shader Generator Initialization failed - Core shader libs path not found", 
		"SampleBrowser::createDummyScene");
  }
  if(mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION) == false) {
    //newViewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
      
    // creates shaders for base material BaseWhite using the RTSS
    Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    baseWhite->setLightingEnabled(false);
    mShaderGenerator->createShaderBasedTechnique(
						 "BaseWhite", 
						 Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
						 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
				       "BaseWhite");
    if(baseWhite->getNumTechniques() > 1) {
      baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(
							       baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
      baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(
								 baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
    }

    // creates shaders for base material BaseWhiteNoLighting using the RTSS
    mShaderGenerator->createShaderBasedTechnique(
						 "BaseWhiteNoLighting", 
						 Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
						 Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
				       "BaseWhiteNoLighting");
    Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
    if(baseWhite->getNumTechniques() > 1) {
      baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(
									 baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
      baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(
									   baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
    }
  }
#endif // INCLUDE_RTSHADER_SYSTEM



  loadResources();

  Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing singleton classes ***");
  //-------------------------------------------------------------------------------------
  Ogre::LogManager::getSingletonPtr()->logMessage("    ShaderManager ");
  new CShaderManager(mRoot->getRenderSystem());
  Ogre::LogManager::getSingletonPtr()->logMessage("    GameSate ");
  m_pGameState = new CGameState();
  Ogre::LogManager::getSingletonPtr()->logMessage("    PauseManager ");
  new CPauseManager();
  Ogre::LogManager::getSingletonPtr()->logMessage("    SaveStateManager ");
  new CSaveStateManager();
  Ogre::LogManager::getSingletonPtr()->logMessage("    GUIManager ");
  new CGUIManager(mSceneMgr, *mWindow);
  Ogre::LogManager::getSingletonPtr()->logMessage("    init GameState ");
  m_pGameState->init();

  Ogre::LogManager::getSingletonPtr()->logMessage("    changing GameState to main menu ");
  m_pGameState->changeGameState(CGameState::GS_MAIN_MENU, true);
  if (CSnapshotManager::getSingleton().loadFromSnapshot()) {
    Ogre::LogManager::getSingletonPtr()->logMessage("    snapshot loaded.");
  }
  else {
    Ogre::LogManager::getSingletonPtr()->logMessage("    no snapshot set");
  }

  // call window resized once to adjust settings
  windowResized(mWindow);
}

bool CGame::frameRenderingQueued(const Ogre::FrameEvent& evt) {
  if(mWindow->isClosed())
    return false;

  if(mShutDown) {
    return false;
  }
  if (!mWindow->isActive()) {
    return true;
  }

  mTrayMgr->frameRenderingQueued(evt);

  return true;
}
void CGame::destroyScene() {
  if(!mRoot->hasSceneManager("MainSceneManager"))
    return;

#ifdef INCLUDE_RTSHADER_SYSTEM
  mShaderGenerator->removeSceneManager(mSceneMgr);
#endif
  mSceneMgr->removeRenderQueueListener(mOverlaySystem);
  mWindow->removeAllViewports();
  mRoot->destroySceneManager(mSceneMgr);
  mSceneMgr = NULL;
}
bool CGame::frameStarted(const Ogre::FrameEvent& evt) {
  //Need to capture/update each device
  mInputContext.capture();

  if (evt.timeSinceLastFrame > 0.1) {
    return true;
  }

  if (CGUIManager::getSingletonPtr()) {
    CGUIManager::getSingleton().update(evt.timeSinceLastFrame);
  }
  CPauseManager::getSingleton().update();
  m_pGameState->update(evt.timeSinceLastFrame);
  return true;
}
bool CGame::frameEnded(const Ogre::FrameEvent& evt) {
  return true;
}

//-------------------------------------------------------------------------------------
bool CGame::keyPressed( const OIS::KeyEvent &arg )
{
#ifdef DEBUG_SHOW_OGRE_TRAY
  if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up

  if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
    {
      mTrayMgr->toggleAdvancedFrameStats();
    }
  else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
    {
      if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
	  mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
	  mDetailsPanel->show();
        }
      else
        {
	  mTrayMgr->removeWidgetFromTray(mDetailsPanel);
	  mDetailsPanel->hide();
        }
    }
  else if (arg.key == OIS::KC_T)   // cycle texture filtering mode
    {
      Ogre::String newVal;
      Ogre::TextureFilterOptions tfo;
      unsigned int aniso;

      switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
        {
        case 'B':
	  newVal = "Trilinear";
	  tfo = Ogre::TFO_TRILINEAR;
	  aniso = 1;
	  break;
        case 'T':
	  newVal = "Anisotropic";
	  tfo = Ogre::TFO_ANISOTROPIC;
	  aniso = 8;
	  break;
        case 'A':
	  newVal = "None";
	  tfo = Ogre::TFO_NONE;
	  aniso = 1;
	  break;
        default:
	  newVal = "Bilinear";
	  tfo = Ogre::TFO_BILINEAR;
	  aniso = 1;
        }

      Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
      Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
      mDetailsPanel->setParamValue(9, newVal);
    }
  else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
    {
      Ogre::String newVal;
      Ogre::PolygonMode pm;

      switch (mCamera->getPolygonMode())
        {
        case Ogre::PM_SOLID:
	  newVal = "Wireframe";
	  pm = Ogre::PM_WIREFRAME;
	  break;
        case Ogre::PM_WIREFRAME:
	  newVal = "Points";
	  pm = Ogre::PM_POINTS;
	  break;
        default:
	  newVal = "Solid";
	  pm = Ogre::PM_SOLID;
        }

      Ogre::SceneManagerEnumerator::SceneManagerIterator iterator = Ogre::Root::getSingleton().getSceneManagerIterator();
      while (iterator.hasMoreElements()) {
	Ogre::SceneManager *pSM = iterator.getNext();
	Ogre::SceneManager::CameraIterator camIt = pSM->getCameraIterator();
	while (camIt.hasMoreElements()) {
	  camIt.getNext()->setPolygonMode(pm);
	}
      }
      mDetailsPanel->setParamValue(10, newVal);
    }

  else if(arg.key == OIS::KC_F6)   // refresh all textures
    {
      Ogre::TextureManager::getSingleton().reloadAll();
    }
  else
#endif // DEBUG_SHOW_OGRE_TRAY
  if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
      mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
  else if (arg.key == OIS::KC_R) {
    createResources();
  }
  else if (arg.key == OIS::KC_E) {
    destroyResources();
  }
#ifdef DEBUG_LOAD_SAFE
  if (arg.key == OIS::KC_F1) {
    CSnapshotManager::getSingleton().makeSnapshot();
  }
  else if (arg.key == OIS::KC_F2) {
    CSnapshotManager::getSingleton().loadFromSnapshot();
  }
#endif

  //CEGUI::System &sys = CEGUI::System::getSingleton();
  //sys.getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(arg.key));
  //sys.getDefaultGUIContext().injectChar(arg.text);

  if (mCameraMan) {
    mCameraMan->injectKeyUp(arg);
  }

  return true;
}

bool CGame::keyReleased( const OIS::KeyEvent &arg )
{
  //CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(arg.key));

  if (mCameraMan) {
    mCameraMan->injectKeyDown(arg);
  }
  return true;
}
/*CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID) {
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
  }*/
bool CGame::mouseMoved( const OIS::MouseEvent &arg ) {
#ifdef DEBUG_SHOW_OGRE_TRAY
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
  mTrayMgr->injectMouseMove(arg);
#endif
#endif
  
  return true;
}
bool CGame::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id) {
#ifdef DEBUG_SHOW_OGRE_TRAY
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
  mTrayMgr->injectMouseDown(arg, id);
#endif
#endif

  return true;
}
bool CGame::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
#ifdef DEBUG_SHOW_OGRE_TRAY
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
  mTrayMgr->injectMouseUp(arg, id);
#endif
#endif

  return true;
}
bool CGame::touchMoved(const OIS::MultiTouchEvent& evt) {
#ifdef DEBUG_SHOW_OGRE_TRAY
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
   mTrayMgr->injectMouseMove(evt);
#endif
#endif
  return true;
}
bool CGame::touchPressed(const OIS::MultiTouchEvent& evt) {
#ifdef DEBUG_SHOW_OGRE_TRAY
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
   mTrayMgr->injectMouseDown(evt);
#endif
#endif
  return true;
}
bool CGame::touchReleased(const OIS::MultiTouchEvent& evt) {
#ifdef DEBUG_SHOW_OGRE_TRAY
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
   mTrayMgr->injectMouseUp(evt);
#endif
#endif
  return true;
}
bool CGame::touchCancelled(const OIS::MultiTouchEvent& evt) {
  return true;
}
//Adjust mouse clipping are
void CGame::windowResized(Ogre::RenderWindow* rw) {
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
  if(mInputContext.mMouse) {
    const OIS::MouseState& ms = mInputContext.mMouse->getMouseState();
    ms.width = rw->getWidth();
    ms.height = rw->getHeight();
  }
#endif
  if (CGUIManager::getSingletonPtr()) {
    CGUIManager::getSingleton().resize(CEGUI::Sizef(rw->getWidth(),
						     rw->getHeight()));
  }
}

//Unattach OIS before window shutdown (very important under Linux)
void CGame::windowClosed(Ogre::RenderWindow* rw)
{
}

#ifdef INCLUDE_RTSHADER_SYSTEM

/*-----------------------------------------------------------------------------
  | Initialize the RT Shader system.	
  -----------------------------------------------------------------------------*/
bool CGame:: initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr)  {			
  if (Ogre::RTShader::ShaderGenerator::initialize()) {
    mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

    mShaderGenerator->addSceneManager(sceneMgr);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_NACL && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
    // Setup core libraries and shader cache path.
    Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
    Ogre::StringVector::iterator itGroup = groupVector.begin();
    Ogre::StringVector::iterator itGroupEnd = groupVector.end();
    Ogre::String shaderCoreLibsPath;
    Ogre::String shaderCachePath;
			
    for (; itGroup != itGroupEnd; ++itGroup) {
      Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
      Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
      Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
      bool coreLibsFound = false;

      // Try to find the location of the core shader lib functions and use it
      // as shader cache path as well - this will reduce the number of generated files
      // when running from different directories.
      for (; it != itEnd; ++it) {
	if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos) {
	  shaderCoreLibsPath = (*it)->archive->getName() + "/cache/";
	  shaderCachePath = shaderCoreLibsPath;
	  coreLibsFound = true;
	  break;
	}
      }
      // Core libs path found in the current group.
      if (coreLibsFound) 
	break; 
    }

    // Core shader libs not found -> shader generating will fail.
    if (shaderCoreLibsPath.empty())			
      return false;			
								
#ifdef _RTSS_WRITE_SHADERS_TO_DISK
    // Set shader cache path.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
    shaderCachePath = Ogre::macCachePath();
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    shaderCachePath = Ogre::macCachePath() + "/org.ogre3d.RTShaderCache";
#endif
    mShaderGenerator->setShaderCachePath(shaderCachePath);		
#endif
#endif
    // Create and register the material manager listener if it doesn't exist yet.
    if (mMaterialMgrListener == NULL) {
      mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);
      Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
    }
  }

  return true;
}

/*-----------------------------------------------------------------------------
  | Destroy the RT Shader system.
  -----------------------------------------------------------------------------*/
void CGame::destroyRTShaderSystem() {
  // Restore default scheme.
  Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);

  // Unregister the material manager listener.
  if (mMaterialMgrListener != NULL) {			
    Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
    delete mMaterialMgrListener;
    mMaterialMgrListener = NULL;
  }

  // Destroy RTShader system.
  if (mShaderGenerator != NULL) {				
    Ogre::RTShader::ShaderGenerator::destroy();
    mShaderGenerator = NULL;
  }
}
#endif // INCLUDE_RTSHADER_SYSTEM
void CGame::createResources() {
  if (CGUIManager::getSingletonPtr()) {
    //CGUIManager::getSingleton().createResources();
    CGUIManager::getSingleton().reloadResources();
    //delete CGUIManager::getSingletonPtr();
  }
}
void CGame::destroyResources() {
  if (!CGUIManager::getSingletonPtr()) {
    //CGUIManager::getSingleton().destroyResources();
    //new CGUIManager(mSceneMgr, *mWindow);
  }
}
void CGame::showLoadingBar() {
  assert(mTrayMgr);
  mTrayMgr->showLoadingBar(6, 0);
}
void CGame::hideLoadingBar() {
  assert(mTrayMgr);
  mTrayMgr->hideLoadingBar();
}
