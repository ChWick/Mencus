#ifndef _MESSAGE_HANDLER_HPP_
#define _MESSAGE_HANDLER_HPP_

#include <OgreSingleton.h>
#include <list>
#include "Message.hpp"

class CMessageInjector;

class CMessageHandler
  : public Ogre::Singleton<CMessageHandler> {
private:
  std::list<CMessageInjector*> m_lInjectors;
  std::list<CMessage> m_lMessages;
public:
  static CMessageHandler &getSingleton();
  static CMessageHandler *getSingletonPtr();

  void process();

  void addInjector(CMessageInjector *pInjector) {m_lInjectors.push_back(pInjector);}
  void removeInjector(CMessageInjector *pInjector) {m_lInjectors.remove(pInjector);}

  void addMessage(const CMessage &m);
};

#endif
