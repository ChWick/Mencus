#ifndef _LEVEL_LIST_HPP_
#define _LEVEL_LIST_HPP_

#include <string>
#include <list>
#include <tinyxml2.h>

struct SLevelInfo {
  std::string sLevelFileName;
  std::string sDifficulty;
  std::string sLevelName;
  std::string sLevelDescription;
  std::string sFullInfoText;
};

class CLevelList {
private:
  std::list<SLevelInfo> m_lLevelInfoList;
public:
  CLevelList();
  
  void load();
  void clear();
};

#endif
