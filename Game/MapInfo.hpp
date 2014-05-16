/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef _MAP_INFO_HPP_
#define _MAP_INFO_HPP_

#include <string>
#include <tinyxml2.h>
#include <memory>
#include "OutputStyle.hpp"

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
  bool m_bIsTutorial;
public:
  CMapInfo();
  CMapInfo(const std::string &sFileName, const std::string &sResourceGroup);
  CMapInfo(const CMapInfoConstPtr src);

  void writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStyle) const;

  std::string generateInfoText() const;

  tinyxml2::XMLElement *getEmptyRootNode();
  const tinyxml2::XMLDocument &getDocument() const {return m_Document;}
  bool isValid() const {return m_bValid;}
  bool isTutorial() const {return m_bIsTutorial;}
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
