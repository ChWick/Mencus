#include "Game.hpp"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_DEFAULT_LIBS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#pragma comment(lib,"user32.lib")
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
void android_main(struct android_app* state)
#else
int main(int argc, char *argv[])
#endif
{
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  app_dummy();
#else
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
  delete app;
  return 0;
}
  
#endif
