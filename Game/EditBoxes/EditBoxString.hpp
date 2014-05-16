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

#ifndef _EDIT_BOX_STRING_
#define _EDIT_BOX_STRING_

#include "EditBoxBase.hpp"
#include <string>

class CEditBoxString : public CEditBoxBase {
private:
  std::string &m_sString;
  CEGUI::Editbox *m_pEditbox;
public:
  CEditBoxString(int id,
		 CEGUI::Window *pParent,
		 float fButtonSize,
		 const CEGUI::String &sTitle,
		 std::string &sstring) 
    : CEditBoxBase(id, pParent, fButtonSize, sTitle),
      m_sString(sstring) {

    using namespace CEGUI;
    Editbox *pEditbox = dynamic_cast<Editbox*>(m_pWindow->
					       createChild("OgreTray/Editbox", "EditString"));
    pEditbox->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pEditbox->setSize(USize(UDim(1, 0), UDim(0, fButtonSize)));
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
