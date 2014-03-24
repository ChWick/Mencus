#ifndef _EDIT_BOX_BASE_
#define _EDIT_BOX_BASE_

#include <CEGUI/CEGUI.h>
#include "../PauseCaller.hpp"
#include "EditBoxListener.hpp"

class CEditBoxBase : CPauseCaller {
protected:
  CEGUI::Window *m_pWindow;
  CEditBoxListener *m_pListener;
  const int m_iID;
public:
  CEditBoxBase(int id, CEGUI::Window *pParent, float fButtonSize, const CEGUI::String &sTitle)
    : m_iID(id),
      m_pListener(NULL),
      m_pWindow(NULL) {
    using namespace CEGUI;
    m_pWindow = pParent->createChild("OgreTray/Group");
    m_pWindow->setPosition(UVector2(UDim(0.5, -4 * fButtonSize), UDim(0.5, -2.5 * fButtonSize)));
    m_pWindow->setSize(USize(UDim(0, 8 * fButtonSize), UDim(0, 5 * fButtonSize)));
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
  int getID() const {return m_iID;}
  void setListener(CEditBoxListener *pListener) {m_pListener = pListener;}
  bool onAccect(const CEGUI::EventArgs &args) {
    if (m_pListener) {m_pListener->onEditBoxAccept(this);}
    accepted();
    delete this;
    return true;
  }
  bool onCancel(const CEGUI::EventArgs &args) {
    if (m_pListener) {m_pListener->onEditBoxCancel(this);}
    canceled();
    delete this;
    return true;
  }

protected:
  virtual bool accepted() = 0;
  virtual bool canceled() = 0;
};


#endif
