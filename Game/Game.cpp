#include "Game.hpp"
#include <CEGUI/RendererModules/Ogre/Renderer.h>
#include <CEGUI/RendererModules/Ogre/ImageCodec.h>
#include <CEGUI/RendererModules/Ogre/ResourceProvider.h>
#include <OgreCodec.h>
#include "GUIManager.hpp"
#include "GameState.hpp"

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
  : mRoot(0),
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
    m_pGameState(NULL),
	  mInputManager(NULL),
    mKeyboard(0) {
}
//-------------------------------------------------------------------------------------
CGame::~CGame(void) {
  if (mTrayMgr) delete mTrayMgr;
  if (mCameraMan) delete mCameraMan;
  if (CGameState::getSingletonPtr()) { delete CGameState::getSingletonPtr(); }
  if (CGUIManager::getSingletonPtr()) {delete CGUIManager::getSingletonPtr();}
  if (CEGUI::System::getSingletonPtr()) {CEGUI::OgreRenderer::destroySystem();}
  if (CInputListenerManager::getSingletonPtr()) {delete CInputListenerManager::getSingletonPtr();}

  //Remove ourself as a Window listener
  Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
  windowClosed(mWindow);
  delete mRoot;
}
bool CGame::go(void)
{

  // Platform dependent plugins files
  mPluginsCfg = "plugins";
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  mPluginsCfg += "_windows";
#else
  mPluginsCfg += "_linux";
#endif

#ifdef USE_DEBUG_PLUGINS
  mPluginsCfg += "_d.cfg";
#else
  mPluginsCfg += ".cfg";
#endif

#ifdef USD_DEBUG_RESOURCES
  mResourcesCfg = "resources_d.cfg";
#else
  mResourcesCfg = "resources.cfg";
#endif

  // construct Ogre::Root
  mRoot = new Ogre::Root(mPluginsCfg);

  //-------------------------------------------------------------------------------------
  // setup resources
  // Load resource paths from config file
  Ogre::ConfigFile cf;
  cf.load(mResourcesCfg);

  // Go through all sections & settings in the file
  Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

  Ogre::String secName, typeName, archName;
  while (seci.hasMoreElements())
    {
      secName = seci.peekNextKey();
      Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
      Ogre::ConfigFile::SettingsMultiMap::iterator i;
      for (i = settings->begin(); i != settings->end(); ++i)
        {
	  typeName = i->first;
	  archName = i->second;
	  Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
									 archName, typeName, secName);
        }
    }
  //-------------------------------------------------------------------------------------
  // configure
  // Show the configuration dialog and initialise the system
  // You can skip this and use root.restoreConfig() to load configuration
  // settings if you were sure there are valid ones saved in ogre.cfg
  if(mRoot->restoreConfig() || mRoot->showConfigDialog())
    {
      // If returned true, user clicked OK so initialise
      // Here we choose to let the system create a default rendering window by passing 'true'
      mWindow = mRoot->initialise(true, "Mencus");
    }
  else {
    return false;
  }

  //-------------------------------------------------------------------------------------
  // choose scenemanager
  // Get the SceneManager, in this case a generic one
  //	mSceneMgr = mRoot->createSceneManager(ST_EXTERIOR_CLOSE, "MainSceneManager");
  mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "MainSceneManager");
  //mSceneMgr->addLodListener(new CTestLOD());
  //mSceneMgr->setShadowUseInfiniteFarPlane(false);
  //-------------------------------------------------------------------------------------
  // create camera
  // Create the camera
  mCamera = mSceneMgr->createCamera("GameCam");

  // Position it at 500 in Z direction
  mCamera->setPosition(Ogre::Vector3(0,0,10));
  // Look back along -Z
  mCamera->lookAt(Ogre::Vector3(0,0,0));
  mCamera->setNearClipDistance(0.01f);
  mCamera->setFarClipDistance(400.0f);


  mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controlle
  mCameraMan->setTopSpeed(10);
  //------------------------------------------------------------------------------------
  // create viewports
  // Create one viewport, entire window
  Ogre::Viewport* vp = mWindow->addViewport(mCamera, 0);
  vp->setBackgroundColour(Ogre::ColourValue(1,0,0));
  vp->setClearEveryFrame(true);

  // Alter the camera aspect ratio to match the viewport
  mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
  //-------------------------------------------------------------------------------------
  // Set default mipmap level (NB some APIs ignore this)
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
  //-------------------------------------------------------------------------------------
  // Create any resource listeners (for loading screens)
  //createResourceListener();
  //-------------------------------------------------------------------------------------
  // load resources
  Ogre::StringVector extensions = Ogre::Codec::getExtensions();
  for(Ogre::StringVector::iterator itExt = extensions.begin(); itExt != extensions.end(); ++itExt)
    {
      Ogre::StringVectorPtr names = Ogre::ResourceGroupManager::getSingleton().findResourceNames("General",std::string("*.")+*itExt);
      for(Ogre::StringVector::iterator itName = names->begin(); itName!=names->end(); ++itName)
	{
	  Ogre::ResourceGroupManager::getSingleton().declareResource(*itName,"Texture","General");
	}
    }
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    //-------------------------------------------------------------------------------------
    // Create the scene
    //mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);

    // Create a light
    Ogre::Light* l = mSceneMgr->createLight("MainLight");
    l->setType(Ogre::Light::LT_DIRECTIONAL);
    l->setPosition(20,80,50);
    l->setDiffuseColour(Ogre::ColourValue(1, 1, 1));
    l->setSpecularColour(Ogre::ColourValue(0.2, 0.2, 0.2));
    l->setDirection(Ogre::Vector3( 0, -2, -1 ));
    l->setCastShadows(false);

    //mSceneMgr->setSkyBox(true, "Skybox/Sky");
    //mSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue::Black,1,30,50);
    //-------------------------------------------------------------------------------------
    //create FrameListener
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));
#else
    pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
    pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
    pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true")));
#endif
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    CInputListenerManager *pInputManager = new CInputListenerManager();
    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mKeyboard->setEventCallback(pInputManager);

    pInputManager->addInputListener(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
    mTrayMgr = new OgreBites::SdkTrayManager("InterfaceName", mWindow, NULL, this);
    mTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    mTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    //mTrayMgr->hideCursor();

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


    m_pCEGuiOgreRenderer = &CEGUI::OgreRenderer::bootstrapSystem();
    //m_pCEGuiImageCodec = &CEGUI::OgreRenderer::createOgreImageCodec();
    //m_pCEGuiResourceProvider = &CEGUI::OgreRenderer::createOgreResourceProvider();*/
    CEGUI::ImageManager::setImagesetDefaultResourceGroup("Imagesets");
    CEGUI::Font::setDefaultResourceGroup("Fonts");
    CEGUI::Scheme::setDefaultResourceGroup("Schemes");
    CEGUI::WidgetLookManager::setDefaultResourceGroup("LookNFeel");
    CEGUI::WindowManager::setDefaultResourceGroup("Layouts");

    //new CEGUI::Logger();
    //new CEGUI::WindowManager();
    //new CEGUI::SchemeManager();
    CEGUI::SchemeManager::getSingleton().createFromFile("OgreTray.scheme");
    //CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage("TaharezLook/MouseArrow");

    new CGUIManager();

    mRoot->addFrameListener(this);
    //-------------------------------------------------------------------------------------
    m_pGameState = new CGameState();

    mRoot->startRendering();

    return true;
}

bool CGame::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  if(mWindow->isClosed())
    return false;

  if(mShutDown) {
    return false;
  }
  if (!mWindow->isActive()) {
    return true;
  }

  //Need to capture/update each device
  mKeyboard->capture();

  mTrayMgr->frameRenderingQueued(evt);

  CEGUI::System::getSingleton().injectTimePulse(evt.timeSinceLastFrame);
  mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera

  return true;
}
bool CGame::frameStarted(const Ogre::FrameEvent& evt) {
  if (evt.timeSinceLastFrame > 0.1) {
    return true;
  }

  CGUIManager::getSingleton().update(evt.timeSinceLastFrame);
  m_pGameState->update(evt.timeSinceLastFrame);
  return true;
}
bool CGame::frameEnded(const Ogre::FrameEvent& evt) {
  return true;
}
//-------------------------------------------------------------------------------------
bool CGame::keyPressed( const OIS::KeyEvent &arg )
{
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
  else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
      mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
  else if (arg.key == OIS::KC_ESCAPE)
    {
      mShutDown = true;
    }

  CEGUI::System &sys = CEGUI::System::getSingleton();
  sys.getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(arg.key));
  sys.getDefaultGUIContext().injectChar(arg.text);

  if (mCameraMan) {
    mCameraMan->injectKeyUp(arg);
  }

  return true;
}

bool CGame::keyReleased( const OIS::KeyEvent &arg )
{
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(arg.key));

  if (mCameraMan) {
    mCameraMan->injectKeyDown(arg);
  }
  return true;
}


//Adjust mouse clipping area
void CGame::windowResized(Ogre::RenderWindow* rw)
{
}

//Unattach OIS before window shutdown (very important under Linux)
void CGame::windowClosed(Ogre::RenderWindow* rw)
{
  //Only close for window that created OIS (the main window in these demos)
  if( rw == mWindow )
    {
      if( mInputManager )
        {
	  mInputManager->destroyInputObject( mKeyboard );

	  OIS::InputManager::destroyInputSystem(mInputManager);
	  mInputManager = 0;
        }
    }
}
