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

#ifndef _SNAPSHOT_HPP_
#define _SNAPSHOT_HPP_

#include "GameState.hpp"
#include <tinyxml2.h>
#include <OgreException.h>
#include <OgreStringConverter.h>
#include "XMLHelper.hpp"

#define SNAPSHOT_VERSION 1

//! Saves a snapshot of the game
/**
 * If currently a map, then the current map state will be stored in an 'xml' file
 */
class CSnapshot {
private:
  int m_iVersion;
  CGameState::EGameStates m_eCurrentGameState;
  tinyxml2::XMLDocument m_XMLDoc;
public:
  CSnapshot() : m_iVersion(SNAPSHOT_VERSION) {}

  //! Constructor to create a snapshot from a given file
  CSnapshot(const Ogre::String &sValidPath) {
    m_XMLDoc.LoadFile(sValidPath.c_str());
    if (m_XMLDoc.Error()) {
      throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND,
			    "XMLdoc parsing error with file: " + sValidPath,
			    __FILE__);
    }

    // read the document
    readDocument();
  }

  CSnapshot(const void *pointer, const size_t size) {
    memcpy(&m_eCurrentGameState, pointer, sizeof(CGameState::EGameStates));
    char *pChar = new char[size];
    memcpy(pChar, static_cast<const CGameState::EGameStates*>(pointer) + 1, size - sizeof(CGameState::EGameStates));

    m_XMLDoc.Parse(pChar);
    if (m_XMLDoc.Error()) {
      throw Ogre::Exception(Ogre::Exception::ERR_FILE_NOT_FOUND, "Save state xml document cout not be parsed. Error code"
			    + Ogre::StringConverter::toString(m_XMLDoc.ErrorID())
			    + " File content:\n" + Ogre::String(pChar), __FILE__);
    }
    
    delete [] pChar;


    // read the document
    readDocument();
  }


  void setGameState(CGameState::EGameStates eGameState) {m_eCurrentGameState = eGameState;}
  CGameState::EGameStates getGameState() const {return m_eCurrentGameState;}

  int getVersion() const {return m_iVersion;}
  bool isValidVersion() const {return m_iVersion == SNAPSHOT_VERSION;}

  tinyxml2::XMLDocument &getXMLDocument() {return m_XMLDoc;}
  const tinyxml2::XMLDocument &getXMLDocument() const {return m_XMLDoc;}


  //! Function to store the data of this game state in memory
  /**
   * \brief This function is used by android to store the gamestate when minimizing
   * \param pointer Output for the memory location
   * \param size Output for the memory size
   */
  void saveToMemory(void* &pointer, size_t &size) const {
    size = sizeof(CGameState::EGameStates);
    tinyxml2::XMLPrinter xmlprinter;
    m_XMLDoc.Accept(&xmlprinter);
    Ogre::String xmltext = xmlprinter.CStr();
    size += xmltext.size();
    pointer = malloc(size);
    memcpy(pointer, &m_eCurrentGameState, sizeof(CGameState::EGameStates));
    memcpy(static_cast<CGameState::EGameStates*>(pointer) + 1, xmltext.c_str(), xmltext.size());
  }
private:
  //! call this if u read from a xml document
  /*
   * \brief This function is called from the constructors to check verion of the read document
   *        and to read the game state of this storage
   */
  void readDocument() {
    if (!m_XMLDoc.FirstChildElement("snapshot")) { 
      throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS,
			    "XMLdoc has no snapshot node",
			    __FILE__);
    }

    tinyxml2::XMLElement *pElem = m_XMLDoc.FirstChildElement("snapshot");
    m_eCurrentGameState
      = XMLHelper::EnumAttribute<CGameState::EGameStates>(
					      pElem,
					      "game_state",
					      CGameState::GS_MAIN_MENU);
    
    if (m_eCurrentGameState >= CGameState::EGameStates::GS_COUNT) {
      throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS,
			    "Game state " + Ogre::StringConverter::toString(m_eCurrentGameState) + " is not valid!", __FILE__);
    }
    m_iVersion = XMLHelper::IntAttribute(pElem, "version", -1);

    if (m_iVersion != SNAPSHOT_VERSION) {
      throw Ogre::Exception(Ogre::Exception::ERR_INVALIDPARAMS,
			    "Snapshot version " + Ogre::StringConverter::toString(m_iVersion)
			    + " does not match with the required verion of " 
			    + Ogre::StringConverter::toString(SNAPSHOT_VERSION),
			    __FILE__);
    }
  }
};

#endif
