#ifndef _EDIT_BOX_BASE_
#define _EDIT_BOX_BASE_

#include <CEGUI/CEGUI.h>
#include "../PauseCaller.hpp"

class CEditBoxBase : CPauseCaller {
protected:
  CEGUI::Window *m_pWindow;
public:
  CEditBoxBase(CEGUI::Window *pParent, float fButtonSize, const CEGUI::String &sTitle) {
    using namespace CEGUI;
    m_pWindow = pParent->createChild("OgreTray/Group");
    m_pWindow->setPosition(UVector2(UDim(0.5, -3 * fButtonSize), UDim(0.5, -2 * fButtonSize)));
    m_pWindow->setSize(USize(UDim(0, 6 * fButtonSize), UDim(0, 4 * fButtonSize)));
    m_pWindow->setAlwaysOnTop(true);
    m_pWindow->setText(sTitle);

    Window *pAcceptButton = m_pWindow->createChild("OgreTray/Button", "Accept");
    pAcceptButton->setPosition(UVector2(UDim(0, 0), UDim(1, -fButtonSize)));
    pAcceptButton->setSize(USize(UDim(0.4, 0), UDim(0, fButtonSize)));
    pAcceptButton->setText("Accept");
    pAcceptButton->subscribeEvent(PushButton::EventClicked,
				  Event::Subscriber(&CEditBoxBase::onAccect, this));

    Window *pCancelButton = m_pWindow->createChild("OgreTray/Button", "Cancel");
    pCancelButton->setPosition(UVector2(UDim(0.6, 0), UDim(1, -fButtonSize)));
    pCancelButton->setSize(USize(UDim(0.4, 0), UDim(0, fButtonSize)));
    pCancelButton->setText("Cancel");
    pCancelButton->subscribeEvent(PushButton::EventClicked,
				  Event::Subscriber(&CEditBoxBase::onCancel, this));
    
    pause(PAUSE_MAP_EDITOR);
  }
  ~CEditBoxBase() {
    unpause(PAUSE_MAP_EDITOR);
    m_pWindow->destroy();
  }

  bool onAccect(const CEGUI::EventArgs &args) {
    if (accepted())
      return onCancel(args);

    return true;
  }
  bool onCancel(const CEGUI::EventArgs &args) {
    delete this;
    return true;
  }

protected:
  virtual bool accepted() = 0;
};


#endif
