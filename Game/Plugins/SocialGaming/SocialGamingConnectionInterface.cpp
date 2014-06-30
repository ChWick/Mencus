#include "SocialGamingConnectionInterface.hpp"
#include "MessageHandler.hpp"
#include "MessageTypes.hpp"
#include "Assert.hpp"
#include "Log.hpp"

using namespace SocialGaming;

void CSocialGamingConnectionInterface::changeSignedIn(bool isSignedIn) {
  m_bSignedIn = isSignedIn;

  CMessageHandler::getSingleton().
    addMessage(CMessage(CMessage::MT_SOCIAL_GAMING).
	       setInt(SIGNED_IN_STATE_CHANGED).
	       setBool(isSignedIn));
}
