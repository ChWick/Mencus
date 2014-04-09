#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include <vector>

class CMessage {
public:
  enum EMessageTypes {
    MT_MESSAGE_BOX_PAGE_CHANGED,
  };
protected:
  const unsigned int m_uiType;

  std::vector<int> m_aIntValues;
public:
  CMessage(const unsigned int uiType)
    : m_uiType(uiType) {
  }

  int getInt(unsigned int idx = 0) const {return m_aIntValues[idx];}
  void setInt(int i, unsigned int idx = 0) {m_aIntValues[idx] = i;}
};

#endif
