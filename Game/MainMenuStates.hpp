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

#ifndef _MAIN_MENU_STATES_HPP_
#define _MAIN_MENU_STATES_HPP_

namespace MainMenu {
  enum EState {
    MMS_STATE_NONE = -1,

    MMS_START = 0,
    MMS_GAME,
    MMS_LOAD_GAME,
    MMS_OPTIONS,
    MMS_OPTIONS_VIDEO,
    MMS_OPTIONS_INPUT,
    MMS_GAME_ESCAPE,
    MMS_USER_GAME,
    MMS_MAP_EDITOR,
    MMS_MAP_EDITOR_SELECT_MAP,

    MMS_COUNT,

    MMS_RESULT_CREDITS,
    MMS_RESULT_BACK_TO_GAME,
    MMS_RESULT_EXIT,
    MMS_RESULT_NEW_GAME,
    MMS_RESULT_LOAD_GAME,
    MMS_RESULT_NEW_MAP,
  };
};

#endif
