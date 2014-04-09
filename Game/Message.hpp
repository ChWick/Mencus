#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include <vector>

class CMessage {
public:
  enum EMessageTypes {
    MT_MESSAGE_BOX_PAGE_CHANGED,
  };
  static const unsigned int MAX_INT_VALUES = 2;
protected:
  const unsigned int m_uiType;

  std::vector<int> m_aIntValues;
public:
  CMessage(const unsigned int uiType)
    : m_uiType(uiType),
      m_aIntValues(MAX_INT_VALUES, 0) {
  }
  CMessage(const unsigned int uiType, int i0, int i1 = 0)
    : m_uiType(uiType),
      m_aIntValues(MAX_INT_VALUES) {
    m_aIntValues[0] = i0;
    m_aIntValues[1] = i1;
  }
  CMessage(const CMessage &src) 
    : m_uiType(src.m_uiType),
      m_aIntValues(src.m_aIntValues) {
  }
  unsigned int getType() const {return m_uiType;}

  int getInt(unsigned int idx = 0) const {return m_aIntValues[idx];}
  CMessage &setInt(int i, unsigned int idx = 0) {m_aIntValues[idx] = i; return *this;}
};

#endif
