#ifndef _EDIT_BOX_UINT_VECTOR2_
#define _EDIT_BOX_UINT_VECTOR2__

#include "EditBoxBase.hpp"


class CEditBoxUIntVector2 : public CEditBoxBase {
private:
  const unsigned int m_uiBackupX;
  const unsigned int m_uiBackupY;
  unsigned int &m_uiX;
  unsigned int &m_uiY;
  CEGUI::Spinner *m_pSpinnerX;
  CEGUI::Spinner *m_pSpinnerY;
public:
  CEditBoxUIntVector2(int id, 
		      CEGUI::Window *pParent,
		      float fButtonSize,
		      const CEGUI::String &sTitle,
		      unsigned int &uiX,
		      unsigned int &uiY) 
    : CEditBoxBase(id, pParent, fButtonSize, sTitle),
      m_uiX(uiX),
      m_uiY(uiY),
      m_uiBackupX(uiX),
      m_uiBackupY(uiY) {

    using namespace CEGUI;

    Window *pLabel = m_pWindow->createChild("OgreTray/Label","lx");
    pLabel->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pLabel->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
    pLabel->setText("x");

    pLabel = m_pWindow->createChild("OgreTray/Label","ly");
    pLabel->setPosition(UVector2(UDim(0, 0), UDim(0, fButtonSize)));
    pLabel->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
    pLabel->setText("y");

    Spinner *pSpinner = dynamic_cast<Spinner*>(m_pWindow->
					       createChild("OgreTray/Spinner", "x"));
    pSpinner->setTextInputMode(Spinner::Integer);
    pSpinner->setPosition(UVector2(UDim(0, fButtonSize), UDim(0, 0)));
    pSpinner->setSize(USize(UDim(1, -fButtonSize), UDim(0, fButtonSize)));
    pSpinner->setText(PropertyHelper<unsigned int>::toString(uiX));
    pSpinner->setMinimumValue(0);
    pSpinner->subscribeEvent(Spinner::EventValueChanged,
			     Event::Subscriber(&CEditBoxUIntVector2::onXValueChanged, this));
    m_pSpinnerX = pSpinner;

    pSpinner = dynamic_cast<Spinner*>(m_pWindow->
					       createChild("OgreTray/Spinner", "y"));
    pSpinner->setTextInputMode(Spinner::Integer);
    pSpinner->setPosition(UVector2(UDim(0, fButtonSize), UDim(0, fButtonSize)));
    pSpinner->setSize(USize(UDim(1, -fButtonSize), UDim(0, fButtonSize)));
    pSpinner->setText(PropertyHelper<unsigned int>::toString(uiY));
    pSpinner->setMinimumValue(0);
    m_pSpinnerY = pSpinner;
    pSpinner->subscribeEvent(Spinner::EventValueChanged,
			     Event::Subscriber(&CEditBoxUIntVector2::onYValueChanged, this));
  }
protected:
  bool accepted() {
    m_uiX = floor(m_pSpinnerX->getCurrentValue() + 0.5);
    m_uiY = floor(m_pSpinnerY->getCurrentValue() + 0.5);

    return true;
  }
  bool canceled() {
    m_uiX = m_uiBackupX;
    m_uiY = m_uiBackupY;
    return true;
  }
  bool onXValueChanged(const CEGUI::EventArgs &args) {
    m_uiX = floor(m_pSpinnerX->getCurrentValue() + 0.5);
    return true;
  }
  bool onYValueChanged(const CEGUI::EventArgs &args) {
    m_uiY = floor(m_pSpinnerY->getCurrentValue() + 0.5);
    return true;
  }

};

#endif