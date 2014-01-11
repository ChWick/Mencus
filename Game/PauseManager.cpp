#include "PauseManager.hpp"

template<> CPauseManager* Ogre::Singleton<CPauseManager>::msSingleton = 0;

CPauseManager* CPauseManager::getSingletonPtr(void)
{
  return msSingleton;
}
CPauseManager& CPauseManager::getSingleton(void)
{
  assert( msSingleton );  return ( *msSingleton );
}

void CPauseManager::update() {
  for (auto pCaller : m_lPauseCallers) {

  }
}
