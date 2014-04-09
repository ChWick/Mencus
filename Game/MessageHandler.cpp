#include "MessageHandler.hpp"
#include "Entity.hpp"

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
    for (auto pEntity : m_lEntityInjectors) {
      pEntity->sendMessageToAll(m_lMessages.front());
    }
    m_lMessages.pop_front();
  }
}
void CMessageHandler::addMessage(const CMessage &m) {
  m_lMessages.push_back(m);
}

