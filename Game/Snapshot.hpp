#ifndef _SNAPSHOT_HPP_
#define _SNAPSHOT_HPP_

#include "GameState.hpp"
#include <tinyxml2.h>

//! Saves a snapshot of the game
/**
 * If currently a map, then the current map state will be stored in an 'xml' file
 */
class CSnapshot {
private:
  CGameState::EGameStates m_eCurrentGameState;
  tinyxml2::XMLDocument m_XMLDoc;
public:
  CSnapshot();

  void setGameState(CGameState::EGameStates eGameState) {m_eCurrentGameState = eGameState;}
  CGameState::EGameStates getGameState() const {return m_eCurrentGameState;}

  tinyxml2::XMLDocument &getXMLDocument() {return m_XMLDoc;}
  const tinyxml2::XMLDocument &getXMLDocument() const {return m_XMLDoc;}
};

#endif
