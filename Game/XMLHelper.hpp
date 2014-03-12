#ifndef _XML_HELPER_HPP_
#define _XML_HELPER_HPP_

#include <tinyxml2.h>
#include <assert.h>
#include <string>

namespace XMLHelper {
  bool BoolAttribute(tinyxml2::XMLElement *pElem, const char *pLabel, bool bDefault);
  template <class T>
  T EnumAttribute(tinyxml2::XMLElement *pElem, const char *pLabel, T eDefault) {
    assert(pElem);
    assert(pLabel);
    if (!pElem->Attribute(pLabel)) {return eDefault;}
    return static_cast<T>(pElem->IntAttribute(pLabel));
  }

};





#endif
