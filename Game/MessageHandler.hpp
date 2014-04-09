#ifndef _MESSAGE_HANDLER_HPP_
#define _MESSAGE_HANDLER_HPP_

#include <OgreSingleton.h>
#include <list>
#include "Message.hpp"

class CEntity;

class CMessageHandler
  : public Ogre::Singleton<CMessageHandler> {
private:
  std::list<CEntity*> m_lEntityInjectors;
  std::list<CMessage> m_lMessages;
public:
  static CMessageHandler &getSingleton();
  static CMessageHandler *getSingletonPtr();

  void process();

  void addInjector(CEntity *pEntity) {m_lEntityInjectors.push_back(pEntity);}
  void removeInjector(CEntity *pEntity) {m_lEntityInjectors.remove(pEntity);}

  void addMessage(const CMessage &m);
};

#endif
