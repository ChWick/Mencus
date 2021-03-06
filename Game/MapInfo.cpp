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

#include "MapInfo.hpp"
#include <OgreResourceGroupManager.h>
#include <tinyxml2.h>
#include <OgreLogManager.h>
#include <XMLHelper.hpp>

using namespace tinyxml2;
using namespace XMLHelper;

const unsigned int MAP_FILE_VERSION = 2;

CMapInfo::CMapInfo()
  : m_sFileName("Newmap"),
    m_eDifficulty(D_EASY),
    m_sName("New map"),
    m_sDescription("A newly created map."),
    m_bValid(true),
    m_bIsTutorial(false) {
  m_Document.Parse(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<map background=\"forest\" sizex=\"20\" sizey=\"20\" version=\"MAP_FILE_VERSION\">"
"<player x=\"0\" y=\"0\" direction=\"1\" hp=\"10\"/>"
"<tiles>"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\" />"
"  <row tiles=\"2 1 1 1 2 1 2 2 2 1 2 1 1 1 2 1 1 2 2 1\" />"
"  <row tiles=\"2 2 1 2 2 1 2 1 1 1 2 2 1 1 2 1 2 1 1 1\" />"
"  <row tiles=\"2 1 2 1 2 1 2 2 2 1 2 1 2 1 2 1 2 1 1 1\" />"
"  <row tiles=\"2 1 1 1 2 1 2 1 1 1 2 1 1 2 2 1 2 1 1 1\" />"
"  <row tiles=\"2 1 1 1 2 1 2 2 2 1 2 1 1 1 2 1 1 2 2 1\" />"
"</tiles>"
"</map>");
}
CMapInfo::CMapInfo(const std::string &sFileName, const std::string &sResourceGroup)
  : m_sFileName(sFileName),
    m_eDifficulty(D_EASY) {

  Ogre::DataStreamPtr dataStream = Ogre::ResourceGroupManager::getSingleton().
    openResource(sFileName + ".xml", sResourceGroup);
  if (dataStream.isNull()) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File " + sFileName + " not found!");
    return;
  }
  m_Document.Parse(dataStream->getAsString().c_str());

  constructor_impl();
}
CMapInfo::CMapInfo(const CMapInfoConstPtr src)
  : m_bValid(src->m_bValid),
    m_sName(src->m_sName),
    m_sDescription(src->m_sDescription),
    m_sFileName(src->m_sFileName),
    m_eDifficulty(src->m_eDifficulty) {
  tinyxml2::XMLPrinter xmlprinter;
  src->m_Document.Accept(&xmlprinter);
  m_Document.Parse(xmlprinter.CStr());

  constructor_impl();
}
void CMapInfo::constructor_impl() {
  if (m_Document.Error()) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File could not be parsed as xml document");
    return;
  }

  XMLElement *pRoot = m_Document.FirstChildElement();
  if (!pRoot) {
    m_bValid = false;
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File has no root node");
    return;
  }
  if (IntAttribute(pRoot, "version") != MAP_FILE_VERSION) {
    Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "File has wrong version code");
    return;
  }
  m_eDifficulty = parseMapDifficlty(Attribute(pRoot, "difficulty", "unknown"));
  m_sName = Attribute(pRoot, "name", "unknown");
  m_sDescription = Attribute(pRoot, "description");
  m_bIsTutorial = BoolAttribute(pRoot, "tutorial", false);

  m_bValid = true;
}
void CMapInfo::writeToXMLElement(tinyxml2::XMLElement *pElem, EOutputStyle eStype) const {
  assert(pElem);
  pElem->SetAttribute("difficulty", toString(m_eDifficulty).c_str());
  pElem->SetAttribute("name", m_sName.c_str());
  pElem->SetAttribute("description", m_sDescription.c_str());
  SetAttribute(pElem, "tutorial", m_bIsTutorial);
}
std::string CMapInfo::generateInfoText() const {
  return "Name: " + m_sName + "\nDifficulty: " + toString(m_eDifficulty) + "\nDescription: " + m_sDescription;
}
tinyxml2::XMLElement *CMapInfo::getEmptyRootNode() {
  m_Document.Clear();
  tinyxml2::XMLElement *pElem =  m_Document.NewElement("map");
  m_Document.InsertEndChild(pElem);

  return pElem;
}
