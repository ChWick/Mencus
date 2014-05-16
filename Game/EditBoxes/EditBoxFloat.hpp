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
