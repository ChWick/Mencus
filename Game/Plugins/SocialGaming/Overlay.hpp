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

#ifndef _SOCIAL_GAMING_OVERLAY_HPP_
#define _SOCIAL_GAMING_OVERLAY_HPP_

#include <CEGUI/CEGUI.h>
#include "Plugins/GUIOverlay.hpp"
#include "MessageInjector.hpp"

namespace SocialGaming {
  class COverlay
    : public CGUIOverlay,
      public CMessageInjector {
  private:
    float m_fTimer;
    int m_iConnectingDots;
    CEGUI::Window *m_pShowButton;
    CEGUI::Window *m_pRoot;
    CEGUI::Window *m_pSignedInState;
  public:
    COverlay(CEGUI::Window *pRoot);
    ~COverlay();

    void update(float tpf);

    void changeTouchButtonSize(float fSize);


    void sendMessageToAll(const CMessage &message);

  private:
    void setSignedIn(bool bSignedIn);

    // button callbacks
    bool onShowSocialGamingSideBar(const CEGUI::EventArgs &args);
    bool onHideSocialGamingSideBar(const CEGUI::EventArgs &args);
    bool onShowOverlay(const CEGUI::EventArgs &args);
    bool onRestart(const CEGUI::EventArgs &args);

    bool onLoginOnStartToggled(const CEGUI::EventArgs &args);
  };
};

#endif
