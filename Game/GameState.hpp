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

#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <OgreSingleton.h>
#include <memory>
#include "MainMenuStates.hpp"
#include "Statistics.hpp"


class CMainMenu;
class CScreenplay;
class CSaveState;
class CMapPack;
class CGUICredits;

class CGameState : public Ogre::Singleton<CGameState> {
public:
  enum EGameStates {
    GS_MAIN_MENU,
    GS_GAME,
    GS_GAME_OVER,
    GS_STATISTICS,
    GS_CREDITS,
    GS_AD,

    GS_COUNT
  };
private:
  EGameStates m_eCurrentGameState;
  EGameStates m_eNextGameState;
  CMainMenu *m_pMainMenu;
  MainMenu::EState m_eMainMenuState;
  CScreenplay *m_pScreenplay;
  const CSaveState *m_pSaveState;
  std::shared_ptr<const CMapPack> m_pMapPack;
  bool m_bForce;
  bool m_bAdShown;

  CGUICredits *m_pCredits;	//!< Holds the credits, if they are shown

  SStatistics m_Statistics;	//!< The current statistics
public:
  static CGameState &getSingleton();
  static CGameState *getSingletonPtr();

  CGameState();
  ~CGameState();

  void init();

  void update(Ogre::Real tpf);

  void changeGameState(EGameStates eNewGameState, bool bNow = false, bool bForce = true);
  void changeGameState(EGameStates eNewGameState, const CSaveState *pState);
  void changeGameState(EGameStates eNewGameState, std::shared_ptr<const CMapPack> pMapPack);
  void changeGameState(EGameStates eNewGameState, MainMenu::EState eMainMenuState, bool bNow = false);

  const CSaveState *getSaveState() const {return m_pSaveState;}
  std::shared_ptr<const CMapPack> getMapPack() const {return m_pMapPack;}
  void setMapPack(std::shared_ptr<const CMapPack> ptr) {m_pMapPack = ptr;}
  EGameStates getCurrentGameState() {return m_eCurrentGameState;}
  CScreenplay *getScreenplay() {return m_pScreenplay;}
  void setAdShown(bool bShown) {m_bAdShown = bShown;}

  SStatistics &getStatistics() {return m_Statistics;}

private:
  void changeGameStateImpl();
};

#endif
