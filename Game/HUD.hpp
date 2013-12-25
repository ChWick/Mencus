#ifndef HUD_HPP
#define HUD_HPP

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
class CHUD : public Ogre::Singleton<CHUD> {
private:
  CEGUI::Window *m_pHudRoot;
public:
  static CHUD &getSingleton();
  static CHUD *getSingletonPtr();

  CHUD(CEGUI::Window *pGUIRoot);

  void update(Ogre::Real tpf);
};

#endif
