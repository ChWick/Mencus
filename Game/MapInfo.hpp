#ifndef _MAP_INFO_HPP_
#define _MAP_INFO_HPP_

#include <string>
#include <tinyxml2.h>
#include <memory>

enum EMapDifficulty {
  D_EASY,
  D_MEDIUM,
  D_HARD,
  D_COUNT
};
class CMapInfo {
public:
  static EMapDifficulty parseMapDifficlty(const std::string &s) {
    if (s == "easy") {return D_EASY;}
    else if (s == "medium") {return D_MEDIUM;}
    else if (s == "hard") {return D_HARD;}
    return D_COUNT;
  }
  static std::string toString(EMapDifficulty d) {
    switch (d) {
    case D_EASY: return "easy";
    case D_MEDIUM: return "medium";
    case D_HARD: return "hard";
    default: return "unknown";
    }
  }
private:
  tinyxml2::XMLDocument m_Document;
  bool m_bValid;
  EMapDifficulty m_eDifficulty;
  std::string m_sName;
  std::string m_sDescription;
  
public:
  CMapInfo(const std::string &sFileName, const std::string &sResourceGroup);

  std::string generateInfoText() const;

  const tinyxml2::XMLDocument &getDocument() const {return m_Document;}
  bool isValid() const {return m_bValid;}
  EMapDifficulty getDifficulty() const {return m_eDifficulty;}
  std::string getDifficultyAsString() const {return toString(m_eDifficulty);}
};

typedef std::shared_ptr<const CMapInfo> CMapInfoConstPtr;
typedef std::shared_ptr<CMapInfo> CMapInfoPtr;

#endif
