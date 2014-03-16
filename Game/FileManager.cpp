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

  struct stat sb;
  int32_t res = stat(dataPath.c_str(), &sb);
  if (0 == res && sb.st_mode & S_IFDIR) {
    // path already exists
  }
  else {
    // create path
    res = mkpath(dataPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
if (res != 0) {
Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "Could not create directory " + dataPath);
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
std::string path(getValidPath(sFileName));
fclose(fopen(path.c_str(), "w+"));
stream.open(path);
if (!stream) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File " + path + " not found");
    return false;
  }
  else {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_NORMAL, "File " + path  + " openened");
  }
return true;
}
