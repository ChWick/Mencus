#include "XMLHelper.hpp"

namespace XMLHelper {
  bool BoolAttribute(tinyxml2::XMLElement *pElem, const char *pLabel, bool bDefault)
    {
    assert(pElem);
    assert(pLabel);
    if (pElem->Attribute(pLabel)) {
      std::string label = pElem->Attribute(pLabel);
      return label == "true" || label == "True" || label == "1";
    }
    else {
      return bDefault;
    }
  }
};
