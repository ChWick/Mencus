#include "FileManager.hpp"
#include "OgreLogManager.h"
#include <iostream>
#include <string>
#include <sys/stat.h>


#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

typedef struct stat Stat;

#ifndef lint
/* Prevent over-aggressive optimizers from eliminating ID string */
const char jlss_id_mkpath_c[] = "@(#)$Id: mkpath.c,v 1.13 2012/07/15 00:40:37 jleffler Exp $";
#endif /* lint */

static int do_mkdir(const char *path, mode_t mode)
{
    Stat            st;
    int             status = 0;

    if (stat(path, &st) != 0)
    {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir(path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
        status = -1;
    }

    return(status);
}

/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int mkpath(const char *path, mode_t mode)
{
    char           *pp;
    char           *sp;
    int             status;
    char           *copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0)
    {
        if (sp != pp)
        {
	  /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir(copypath, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = do_mkdir(path, mode);
    free(copypath);
    return (status);
}

// define paths
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
const std::string CFileManager::DIRECTORY_LEVEL("level/");
#else
const std::string CFileManager::DIRECTORY_LEVEL("../level/user/");
#endif

bool CFileManager::m_bInitialized(false);
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
ANativeActivity *CFileManager::mNativeActivity(NULL);
#endif


std::string CFileManager::getValidPath(const std::string &sFileName,
					 EStorageLocation eLocation) {
  assert(m_bInitialized);
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  ANativeActivity* nativeActivity = mNativeActivity;
  std::string dataPath;
  if (eLocation == SL_INTERNAL) {
    dataPath = nativeActivity->internalDataPath;
  }
  else {
    dataPath = nativeActivity->externalDataPath;
  }

  std::string addPath(sFileName.substr(0, sFileName.rfind("/")));
  std::string path = dataPath;
  if (addPath.size() > 0) {
    path += "/" + addPath;
  }

  struct stat sb;
  int32_t res = stat(path.c_str(), &sb);
  if (0 == res && sb.st_mode & S_IFDIR) {
    // path already exists
  }
  else {
    // create path
    res = mkpath(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (res != 0) {
      // error...
    }
  }

  return dataPath + "/" + sFileName;
#else
  return sFileName;
#endif
}
bool CFileManager::openFile(std::fstream &stream,
			    const std::string &sFileName,
			    EStorageLocation eLocation) {
  assert(m_bInitialized);
  std::string path(getValidPath(sFileName, eLocation));
  stream.open(path);
  if (!stream) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "Creating file at path: " + path);
    if (FILE *f = fopen(path.c_str(), "w+")) {
      fclose(f);
      stream.open(path);
    }
    if (!stream) {
      Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File " + path + " not found");
      return false;
    }
  }
  if (stream) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "File " + path  + " openened");
  }
  return true;
}
