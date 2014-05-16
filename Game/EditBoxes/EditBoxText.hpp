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

#ifndef _EDIT_BOX_TEXT_
#define _EDIT_BOX_TEXT_

#include "EditBoxBase.hpp"
#include <string>

class CEditBoxText : public CEditBoxBase {
private:
  std::string &m_sString;
  CEGUI::MultiLineEditbox *m_pEditbox;
public:
  CEditBoxText(int id,
	       CEGUI::Window *pParent,
	       float fButtonSize,
	       const CEGUI::String &sTitle,
	       std::string &sstring) 
    : CEditBoxBase(id, pParent, fButtonSize, sTitle),
      m_sString(sstring) {

    using namespace CEGUI;
    MultiLineEditbox *pEditbox = dynamic_cast<MultiLineEditbox*>(m_pWindow->
								 createChild("OgreTray/MultiLineEditbox", "EditString"));
    pEditbox->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pEditbox->setSize(USize(UDim(1, 0), UDim(1, -fButtonSize)));
    pEditbox->setText(m_sString.c_str());
    m_pEditbox = pEditbox;
  }
protected:
  bool accepted() {
    m_sString = m_pEditbox->getText().c_str();

    return true;
  }
  bool canceled() {
    return true;
  }
};

#endif
