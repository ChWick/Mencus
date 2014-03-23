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
