#include "MainMenu.hpp"

#include <OgreStringConverter.h>
#include <iostream>

using namespace std;
using namespace CEGUI;

template<> CMainMenu *Ogre::Singleton<CMainMenu>::msSingleton = 0;

CMainMenu *CMainMenu::getSingletonPtr() {
  return msSingleton;
}
CMainMenu &CMainMenu::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CMainMenu::CMainMenu(CEGUI::Window *pGUIRoot)
  : m_vSlots(NUM_SLOTS)
{
  m_pMMRoot = pGUIRoot->createChild("DefaultWindow", "MainMenuRoot");
  m_pMMRoot->setInheritsAlpha(false);
  m_pMMRoot->setAlpha(1);
}
void CMainMenu::update(Ogre::Real tpf) {
}
