#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include <CEGUI/CEGUI.h>
#include <OgreSingleton.h>
#include <vector>

class CMainMenu : public Ogre::Singleton<CMainMenu> {
public:
  enum EMainMenuState {
    MMS_START,
    MMS_NEW_GAME,

  };
  static const int NUM_SLOTS = 5;
private:
  CEGUI::Window *m_pMMRoot;
  std::vector<CEGUI::Windows *> m_vSlots;
public:
  static CHUD &getSingleton();
  static CHUD *getSingletonPtr();

  CMainMenu(CEGUI::Window *pGUIRoot);

  void update(Ogre::Real tpf);
};

#endif // MAIN_MENU_HPP
