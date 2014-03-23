#ifndef _EDIT_BOX_FLOAT_
#define _EDIT_BOX_FLOAT_

#include "EditBoxBase.hpp"



class CEditBoxFloat : public CEditBoxBase {
private:
  float &m_fValue;
  CEGUI::Spinner *m_pSpinner;
public:
  CEditBoxFloat(int id,
		CEGUI::Window *pParent,
		float fButtonSize,
		const CEGUI::String &sTitle,
		float &fValue) 
    : CEditBoxBase(id, pParent, fButtonSize, sTitle),
      m_fValue(fValue) {

    using namespace CEGUI;
    Spinner *pSpinner = dynamic_cast<Spinner*>(m_pWindow->
					       createChild("OgreTray/Spinner", "EditFloat"));
    pSpinner->setTextInputMode(Spinner::FloatingPoint);
    pSpinner->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pSpinner->setSize(USize(UDim(1, 0), UDim(0, fButtonSize)));
    pSpinner->setText(PropertyHelper<float>::toString(fValue));
    pSpinner->setMinimumValue(0);
    m_pSpinner = pSpinner;
  }
protected:
  bool accepted() {
    m_fValue = m_pSpinner->getCurrentValue();

    return true;
  }
  bool canceled() {
    return true;
  }
};

#endif
