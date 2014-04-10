#include "EventEmitter.hpp"
#include <OgreException.h>
#include "XMLHelper.hpp"

using namespace XMLHelper;
using namespace EventEmitter;

namespace EventEmitter {

std::string toString(EEmitter eEmitter) {
  switch (eEmitter) {
  case EMIT_ON_CREATE:
    return "create";
  case EMIT_ON_DESTROY:
    return "destroy";
  case EMIT_ON_USER:
    return "user";
  case EMIT_ON_MESSAGE_BOX_PAGE_CHANGE:
    return "message_page_chagned";
  }
  
  throw Ogre::Exception(0, "Emitter type " + Ogre::StringConverter::toString(eEmitter) + " could not be converted to a string", __FILE__);
}
EEmitter parseEmitter(const std::string &sString) {
  if (sString == "create") {return EMIT_ON_CREATE;}
  else if (sString == "destroy") {return EMIT_ON_DESTROY;}
  else if (sString == "user") {return EMIT_ON_USER;}
  else if (sString == "message_page_chagned") {return EMIT_ON_MESSAGE_BOX_PAGE_CHANGE;}

  throw Ogre::Exception(0, "Emitter type " + sString + " could not be converted to a string", __FILE__);
}

CEmitter *CCreator::create(const tinyxml2::XMLElement *pElem) {
  switch (parseEmitter(Attribute(pElem, "emitter"))) {
  case EMIT_ON_CREATE:
    return new COnCreate(pElem);
  case EMIT_ON_DESTROY:
    return new COnDestroy(pElem);
  case EMIT_ON_USER:
    return new COnUser(pElem);
  case EMIT_ON_MESSAGE_BOX_PAGE_CHANGE:
    return new COnMessageBoxPageChange(pElem);
  default:
    throw Ogre::Exception(0, "Emitter could not be created", __FILE__);
  }
}

};
