#ifndef _LEVEL_STATE_HPP_
#define _LEVEL_STATE_HPP_

#include <string>
#include <vector>
#include "Statistics.hpp"
#include <tinyxml2.h>

class CLevelState {
private:
  static std::vector<SStatistics> m_vLevelStatistics;
  static bool m_bLoaded;
  static tinyxml2::XMLDocument m_XMLDocument;
public:
  static bool has(const std::string &sFileName);
  static SStatistics &get(const std::string &sFileName);
  static bool levelAccomplished(const std::string &sFileName);

private:
  static void read();
  static void write();
};

#endif
