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

class CMapInfo;
typedef std::shared_ptr<const CMapInfo> CMapInfoConstPtr;
typedef std::shared_ptr<CMapInfo> CMapInfoPtr;

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
  std::string m_sFileName;
public:
  CMapInfo(const std::string &sFileName, const std::string &sResourceGroup);
  CMapInfo(const CMapInfoConstPtr src);

  std::string generateInfoText() const;

  tinyxml2::XMLElement *getEmptyRootNode();
  const tinyxml2::XMLDocument &getDocument() const {return m_Document;}
  bool isValid() const {return m_bValid;}
  void setDifficulty(EMapDifficulty eDiff) {m_eDifficulty = eDiff;}
  EMapDifficulty getDifficulty() const {return m_eDifficulty;}
  std::string getDifficultyAsString() const {return toString(m_eDifficulty);}

  std::string getXMLText();
  const std::string &getFileName() const {return m_sFileName;}
  void setFileName(const std::string &sFileName) {m_sFileName = sFileName;}
  std::string &getName() {return m_sName;}
  std::string &getFileName() {return m_sFileName;}
  std::string &getDescription() {return m_sDescription;}

private:
  void constructor_impl();
};


#endif
