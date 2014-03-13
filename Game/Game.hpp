#ifndef GAME_HPP
#define GAME_HPP

#include <Ogre.h>
#include <OgreSingleton.h>
#include "InputListener.hpp"
#include "dependencies/OgreSdkUtil/SdkCameraMan.h"
#include "dependencies/OgreSdkUtil/SdkTrays.h"
#include "OgreFileSystemLayer.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include <android_native_app_glue.h>
#include "Android/OgreAPKFileSystemArchive.h"
#include "Android/OgreAPKZipArchive.h"
#endif

#include "ShaderGenerator.hpp"

class CGameState;

class CGame : public CInputListener,
              public Ogre::FrameListener,
              public Ogre::WindowEventListener,
              protected OgreBites::SdkTrayListener,
              public Ogre::Singleton<CGame> {
private:
  CGameState *m_pGameState;

#ifdef INCLUDE_RTSHADER_SYSTEM
  Ogre::RTShader::ShaderGenerator*	    mShaderGenerator;	  //!< The Shader generator instance.
  ShaderGeneratorTechniqueResolverListener* mMaterialMgrListener; //!< Shader generator material manager listener.	
#endif // INCLUDE_RTSHADER_SYSTEM
public:
  CGame(void);
  virtual ~CGame(void);

  static CGame &getSingleton();
  static CGame *getSingletonPtr();

  OIS::Keyboard* getKeyboard() {return mInputContext.mKeyboard;}
  OIS::Mouse *getMouse() {return mInputContext.mMouse;}
  const OgreBites::InputContext &getInputContext() const {return mInputContext;}

  OgreBites::ParamsPanel* getDetailsPanel() {return mDetailsPanel;}
  Ogre::FileSystemLayer* getFileSystemLayer() {return mFSLayer;}

  void go();
  void initApp();
  void closeApp();

  void shutDown() { mShutDown = true; }
  
  void createResources();
  void destroyResources();

  void showLoadingBar();
  void hideLoadingBar();

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  void initAppForAndroid(Ogre::RenderWindow *window, struct android_app* app, OIS::MultiTouch *mouse, OIS::Keyboard *keyboard) {
    Ogre::LogManager::getSingletonPtr()->logMessage("Initializing App for Android");
    assert(mouse);
    assert(keyboard);
    mWindow = window;
    mInputContext.mMultiTouch = mouse;
    mInputContext.mKeyboard = keyboard;

    if(app != NULL) {
      mNativeActivity = app->activity;
      mAssetMgr = app->activity->assetManager;
      Ogre::ArchiveManager::getSingleton().addArchiveFactory(
        new Ogre::APKFileSystemArchiveFactory(app->activity->assetManager));
      Ogre::ArchiveManager::getSingleton().addArchiveFactory(
	new Ogre::APKZipArchiveFactory(app->activity->assetManager));
    }
  }
#endif

  void writeToFile(const Ogre::String &fileName, const Ogre::String &text) {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    writeToInternalStorageFile(fileName, text);
#else
    std::ofstream stream(fileName);
    stream << text;
    stream.close();
#endif
  }
  Ogre::String getFileAsText(const Ogre::String &fileName) {
    #if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    return getTextFromInternalStorageFile(fileName);
#else
    std::ifstream stream(fileName);
    std::string content( (std::istreambuf_iterator<char>(stream) ),
                       (std::istreambuf_iterator<char>()    ) );
    return content;
#endif
  }
protected:
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
  Ogre::OverlaySystem *mOverlaySystem;
#endif
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
  OgreBites::InputContext mInputContext;

  // Ogre::FrameListener
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
  virtual bool frameStarted(const Ogre::FrameEvent& evt);
  virtual bool frameEnded(const Ogre::FrameEvent& evt);

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

  // Ogre::WindowEventListener
  virtual void windowResized(Ogre::RenderWindow* rw);
  virtual void windowClosed(Ogre::RenderWindow* rw);
private:
  void createRoot();
  void setup();
  void setupInput(bool nograb = false);
  void createInputDevices();
  void shutdown();
  void shutdownInput();
  void locateResources();
  void loadResources();
  Ogre::RenderWindow *createWindow();
  void createScene();
  void destroyScene();

  bool oneTimeConfig();
private:
  Ogre::FileSystemLayer* mFSLayer; // File system abstraction layer
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  Ogre::DataStreamPtr openAPKFile(const Ogre::String& fileName) {
    Ogre::DataStreamPtr stream;
    AAsset* asset = AAssetManager_open(mAssetMgr, fileName.c_str(), AASSET_MODE_BUFFER);
    if(asset) {
      off_t length = AAsset_getLength(asset);
      void* membuf = OGRE_MALLOC(length, Ogre::MEMCATEGORY_GENERAL);
      memcpy(membuf, AAsset_getBuffer(asset), length);
      AAsset_close(asset);
      
      stream = Ogre::DataStreamPtr(new Ogre::MemoryDataStream(membuf, length, true, true));
    }
    return stream;
  }
  Ogre::String getTextFromInternalStorageFile(const Ogre::String &fileName) {
    ANativeActivity* nativeActivity = mNativeActivity;                              
    const char* internalPath = nativeActivity->internalDataPath;
    std::string dataPath(internalPath);                               
    // internalDataPath points directly to the files/ directory                                  
    std::string configFile = dataPath + "/" + fileName;

    FILE* f = std::fopen(configFile.c_str(), "r");// Determine file size
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);

    char* where = new char[size];

    rewind(f);
    fread(where, sizeof(char), size, f);

    Ogre::String out(where);
    delete[] where;
    return out;
  }
  void writeToInternalStorageFile(const Ogre::String &fileName, const Ogre::String &text) {
    ANativeActivity* nativeActivity = mNativeActivity;                              
    const char* internalPath = nativeActivity->internalDataPath;
    std::string dataPath(internalPath);                               
    // internalDataPath points directly to the files/ directory                                  
    std::string configFile = dataPath + "/" + fileName;

    // sometimes if this is the first time we run the app 
    // then we need to create the internal storage "files" directory
    struct stat sb;
    int32_t res = stat(dataPath.c_str(), &sb);
    if (0 == res && sb.st_mode & S_IFDIR) {
      Ogre::LogManager::getSingleton().logMessage("'files/' dir already in app's internal data storage.");
    }
    else {
        res = mkdir(dataPath.c_str(), 0770);
    }

    if (0 == res) {
        // test to see if the config file is already present
        res = stat(configFile.c_str(), &sb);
        if (0 == res && sb.st_mode & S_IFREG) {
            Ogre::LogManager::getSingleton().logMessage("Application config file already present");
        }
        else
        {
            Ogre::LogManager::getSingleton().logMessage("Application config file does not exist. Creating it ...");
            // read our application config file from the assets inside the apk
            // save the config file contents in the application's internal storage
            Ogre::LogManager::getSingleton().logMessage("Reading config file using the asset manager.\n");

            FILE* appConfigFile = std::fopen(configFile.c_str(), "w+");
            if (NULL == appConfigFile) {
	      throw Ogre::Exception(0, "Could not create app configuration file.\n", __FILE__);
            }
            else
            {
                Ogre::LogManager::getSingleton().logMessage("App config file created successfully. Writing config data ...\n");
                res = std::fwrite(text.c_str(), sizeof(char), text.size(), appConfigFile);
                if (text.size() != res) {
		  throw Ogre::Exception(0, "Error generating app configuration file.\n", __FILE__);
                }
            }
            std::fclose(appConfigFile);
        }
    }
  }
  AAssetManager* mAssetMgr;       // Android asset manager to access files inside apk
  ANativeActivity* mNativeActivity;
#endif
#ifdef INCLUDE_RTSHADER_SYSTEM
  bool initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr);
  void destroyRTShaderSystem();
#endif
};

#endif // GAME_HPP
