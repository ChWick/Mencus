#ifndef _HUD_ELEMENT_HPP_
#define _HUD_ELEMENT_HPP_

#include "HUD.hpp"

class CHUDElement {
public:
  CHUDElement() {
    CHUD::getSingleton().getElements().push_back(this);
  }
  virtual ~CHUDElement() {
    CHUD::getSingleton().getElements().remove(this);
  }
};

#endif
