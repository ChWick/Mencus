#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include <vector>
#include <string>
#include <OgreException.h>

class CMessage {
public:
  enum EMessageTypes {
    MT_MESSAGE_UNSET,
    MT_MAP_DESTROYED,
    MT_TOGGLE_TOUCH_INPUT_BLINK,
    MT_MESSAGE_BOX_PAGE_CHANGED,
  };
  
  static std::string toString(EMessageTypes eMT) {
    switch (eMT) {
    case MT_MESSAGE_UNSET:
      return "unset";
    case MT_MAP_DESTROYED:
      return "map_destroyed";
    case MT_TOGGLE_TOUCH_INPUT_BLINK:
      return "toggle_touch_input_blink";
    case MT_MESSAGE_BOX_PAGE_CHANGED:
      return "message_box_page_changed";
    }

    throw Ogre::Exception(0, "Message type could not be converted to string", __FILE__);
  }
  static EMessageTypes parseMessageType(const std::string &s) {
    if (s == "unset") {return MT_MESSAGE_UNSET;}
    else if (s == "map_destroyed") {return MT_MAP_DESTROYED;}
    else if (s == "toggle_touch_input_blink") {return MT_TOGGLE_TOUCH_INPUT_BLINK;}
    else if (s == "message_box_page_changed") {return MT_MESSAGE_BOX_PAGE_CHANGED;}

    throw Ogre::Exception(0, "Message type could not be parsed: " + s, __FILE__);
  }
  

  static const unsigned int MAX_INT_VALUES = 2;
  static const unsigned int MAX_BOOL_VALUES = 2;
protected:
  unsigned int m_uiType;

  std::string m_sID;
  std::vector<int> m_aIntValues;
  std::vector<bool> m_aBoolValues;
public:
  CMessage(const unsigned int uiType = MT_MESSAGE_UNSET)
    : m_uiType(uiType),
      m_aIntValues(MAX_INT_VALUES, 0),
      m_aBoolValues(MAX_BOOL_VALUES, false) {
  }
  CMessage(const unsigned int uiType, int i0, int i1 = 0)
    : m_uiType(uiType),
      m_aIntValues(MAX_INT_VALUES),
      m_aBoolValues(MAX_BOOL_VALUES, false) {
    m_aIntValues[0] = i0;
    m_aIntValues[1] = i1;
  }
  CMessage(const CMessage &src) 
    : m_uiType(src.m_uiType),
      m_sID(src.m_sID),
      m_aIntValues(src.m_aIntValues),
      m_aBoolValues(src.m_aBoolValues) {
  }
  unsigned int getType() const {return m_uiType;}
  CMessage &setType(unsigned int uiType) {m_uiType = uiType; return *this;}

  int getInt(unsigned int idx = 0) const {return m_aIntValues[idx];}
  CMessage &setInt(int i, unsigned int idx = 0) {m_aIntValues[idx] = i; return *this;}

  bool getBool(unsigned int idx = 0) const {return m_aBoolValues[idx];}
  CMessage &setBool(bool b, unsigned int idx = 0) {m_aBoolValues[idx] = b; return *this;}

  const std::string &getID() const {return m_sID;}
  CMessage &setID(const std::string &sID) {m_sID = sID; return *this;}
};

#endif
