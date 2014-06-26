#include "SocialGaming.hpp"
#include "GlobalBuildDefines.hpp"

#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
#include "GameCircle.hpp"
#else
#include "SocialGamingConnectionInterface.hpp"
#endif

using namespace SocialGaming;

CSocialGaming CSocialGaming::m_Singleton;

CSocialGaming::CSocialGaming() {
#if MENCUS_USE_AMAZON_GAME_CIRCLE == 1
  m_pConnection = new GameCircle::CConnectionInterface();
#else
  // dummy to avoid nullptr checks
  m_pConnection = new CSocialGamingConnectionInterface();
#endif
}
