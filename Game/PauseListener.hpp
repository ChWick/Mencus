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

#ifndef PAUSE_LISTENER_HPP
#define PAUSE_LISTENER_HPP

#include "PauseManager.hpp"

class CPauseListener {
public:
  CPauseListener() {
    CPauseManager::getSingleton().addListener(this);
  }
  virtual ~CPauseListener() {
    CPauseManager::getSingleton().removeListener(this);
  }

  virtual void playerInputPauseChanged(bool bPause) {}
  virtual void playerMovePauseChanged(bool bPause) {}
  virtual void enemyMovePauseChanged(bool bPause) {}
  virtual void videoPauseChanged(bool bPause) {}
  virtual void mapUpdatePauseChanged(bool bPause) {}
  virtual void mapRenderPauseChanged(bool bPause) {}
  virtual void screenplayPauseChanged(bool bPause) {}
  virtual void mapEditorUpdatePauseChanged(bool bPause) {}
};

#endif // PAUSE_LISTENER_HPP
