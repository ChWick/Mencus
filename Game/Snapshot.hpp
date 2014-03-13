#ifndef _SNAPSHOT_HPP_
#define _SNAPSHOT_HPP_

#include "GameState.hpp"
#include <tinyxml2.h>
#include <OgreException.h>
#include <OgreStringConverter.h>

//! Saves a snapshot of the game
/**
 * If currently a map, then the current map state will be stored in an 'xml' file
 */
class CSnapshot {
private:
  CGameState::EGameStates m_eCurrentGameState;
  tinyxml2::XMLDocument m_XMLDoc;
public:
  CSnapshot() {}
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
  }
  

  void setGameState(CGameState::EGameStates eGameState) {m_eCurrentGameState = eGameState;}
  CGameState::EGameStates getGameState() const {return m_eCurrentGameState;}

  tinyxml2::XMLDocument &getXMLDocument() {return m_XMLDoc;}
  const tinyxml2::XMLDocument &getXMLDocument() const {return m_XMLDoc;}

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
};

#endif
