#include "Game.hpp"
#include "SnapshotManager.hpp"
#include <CEGUI/CEGUI.h>
#include <OIS.h>
#include "FileManager.hpp"
#include "Settings.hpp"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_DEFAULT_LIBS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#pragma comment(lib,"user32.lib")
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include "Android/Android.hpp"
#include <jni.h>

CGame* OgreAndroidBridge::mGame = NULL;
AndroidInputInjector* OgreAndroidBridge::mInputInjector = NULL;
AndroidMultiTouch* OgreAndroidBridge::mTouch = NULL;
AndroidKeyboard* OgreAndroidBridge::mKeyboard = NULL;
Ogre::RenderWindow* OgreAndroidBridge::mRenderWnd = NULL;
Ogre::Root* OgreAndroidBridge::mRoot = NULL;
bool OgreAndroidBridge::mInit = false;
bool OgreAndroidBridge::m_bRenderPaused = true;
CSnapshot *OgreAndroidBridge::m_pSnapshot = NULL;
ANativeActivity *OgreAndroidBridge::mActivity = NULL;

#   ifdef OGRE_STATIC_LIB
Ogre::StaticPluginLoader* OgreAndroidBridge::mStaticPluginLoader = NULL;
#   endif

#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
void android_main(struct android_app* state)
#else
int main(int argc, char *argv[])
#endif
{
  new CSnapshotManager();
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID

  LOGI("Starting");
  app_dummy();

  LOGI("Init FileManager");
  CFileManager::init(state->activity);
  LOGI("Init Settings");
  new CSettings();

  LOGI("Initialize");
  OgreAndroidBridge::init(state);
  LOGI("Go");
  OgreAndroidBridge::go(state);
  LOGI("End");
#else
  CFileManager::init();
  new CSettings();
  CSnapshotManager::getSingleton().createFromFile("snapshot.xml");
  // Create application object
  CGame *app = new CGame();

  try {
    app->go();
  } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cerr << "An exception has occured: " <<
      e.getFullDescription().c_str() << std::endl;
    std::cout << "An exception hat occured: " <<
      e.getFullDescription().c_str() << std::endl;
#endif
  }
  catch (CEGUI::Exception &e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    MessageBox( NULL, e.getMessage().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cerr << "An exception has occured: " <<
      e.getMessage().c_str() << std::endl;
#endif
  }
  catch (...) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    MessageBox( NULL, "Unknown Error", "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cerr << "An exception has occured: " <<
      "Unknown Error" << std::endl;
#endif
  }
#endif

  // For all platforms
  if (CSnapshotManager::getSingletonPtr()) {delete CSnapshotManager::getSingletonPtr();}
  if (CSettings::getSingletonPtr()) {delete CSettings::getSingletonPtr();}

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  delete app;
  return 0;
#endif
}
