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
    CEGUI::Window *m_pShowButton;
    CEGUI::Window *m_pRoot;
    CEGUI::Window *m_pSignedInState;
  public:
    COverlay(CEGUI::Window *pRoot);
    ~COverlay();


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
