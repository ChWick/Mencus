#ifndef _FILE_MANAGER_HPP_
#define _FILE_MANAGER_HPP_

#include <fstream>
#include <string>
#include <OgrePlatform.h>
#include <OgreLogManager.h>
#include <OgreDataStream.h>

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include <android_native_app_glue.h>
#endif

class CFileManager {
private:
  static bool m_bInitialized;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  static ANativeActivity *mNativeActivity;
#endif
public:
  static const std::string DIRECTORY_LEVEL;
  enum EStorageLocation {
    SL_PACKET,
    SL_INTERNAL,
    SL_EXTERNAL
  };
  static void init(
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
		   ANativeActivity *pNativeActivity
#endif
		   ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    assert(pNativeActivity);
    mNativeActivity = pNativeActivity;
#endif
    m_bInitialized = true;
  }
  static bool openFile(std::fstream &stream,
		       const std::string &sFileName,
		       std::ios_base::openmode mode,
		       EStorageLocation eLocation = SL_INTERNAL);
  static void deleteFile(const std::string &sFileName,
			 EStorageLocation eLocation);
  static std::string getValidPath(const std::string &sFileName,
				  EStorageLocation eLocation = SL_INTERNAL);
  static Ogre::DataStreamPtr openDataStream(const std::string& fileName);
};

#endif
