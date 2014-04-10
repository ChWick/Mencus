#ifndef _MESSAGE_INJECTOR_HPP_
#define _MESSAGE_INJECTOR_HPP_

class CMessage;

class CMessageInjector {
public:
  virtual void sendMessageToAll(const CMessage &message) = 0;
};

#endif
