#include "MessageHandler.hpp"
#include "MessageInjector.hpp"

template<> CMessageHandler *Ogre::Singleton<CMessageHandler>::msSingleton = 0;

CMessageHandler *CMessageHandler::getSingletonPtr() {
  return msSingleton;
}
CMessageHandler &CMessageHandler::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}
void CMessageHandler::process() {
  while (m_lMessages.size() > 0) {
    for (auto pInjector : m_lInjectors) {
      pInjector->sendMessageToAll(m_lMessages.front());
    }
    m_lMessages.pop_front();
  }
}
void CMessageHandler::addMessage(const CMessage &m) {
  m_lMessages.push_back(m);
}

