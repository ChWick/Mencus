#include "Game.hpp"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_DEFAULT_LIBS
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
      CGame *app = new CGame();

      try {
	app->go();
      } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
            std::cout << "An exception hat occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }
		catch (CEGUI::Exception &e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA( NULL, e.getMessage().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getMessage().c_str() << std::endl;
#endif
		}
		 catch (...) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA( NULL, "Unknown Error", "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                "Unknown Error" << std::endl;
#endif
		}
      delete app;
      return 0;
    }
  
#ifdef __cplusplus
}
#endif
