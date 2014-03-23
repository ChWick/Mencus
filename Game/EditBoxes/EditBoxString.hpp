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
