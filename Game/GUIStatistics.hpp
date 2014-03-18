#ifndef _GUISTATISTICS_HPP_
#define _GUISTATISTICS_HPP_

#include <OgreSingleton.h>
#include <CEGUI/CEGUI.h>
#include "InputListener.hpp"
#include "Statistics.hpp"

class CGUIStatistics : public Ogre::Singleton<CGUIStatistics>, public CInputListener {
private:
  enum EButtons {
    BT_RETRY = 0,
    BT_TO_MENU,
    BT_COUNT
  };
  enum ELabels {
    L_TIME = 0,
    L_HITPOINTS,
    L_MANAPOINTS,
    L_LOST_HITPOINTS,
    L_USED_MANAPOINTS,
    L_COUNT,
  };

  CEGUI::Window *m_pStatisticsRoot;
  CEGUI::Window *m_pButtons[BT_COUNT];
  int m_iSelectedSlot;
public:
  static CGUIStatistics &getSingleton();
  static CGUIStatistics *getSingletonPtr();

  CGUIStatistics(CEGUI::Window *pRoot);
  ~CGUIStatistics();

  void update(Ogre::Real tpf);

  virtual bool keyPressed(const OIS::KeyEvent &arg);

  void show() { m_pStatisticsRoot->setVisible(true); setInputListenerEnabled(true); }
  void hide() { m_pStatisticsRoot->setVisible(false); setInputListenerEnabled(false); }

  void activateButton(int iBtn);
  void showStatistics(const SStatistics &stats);

  void resize(const CEGUI::String &smallFont, const CEGUI::String &bigfont);

private:
  void createLabel(int iLabel, CEGUI::Window *pParent, bool bData);
  bool onRetryClicked(const CEGUI::EventArgs&);
  bool onToMenuClicked(const CEGUI::EventArgs&);
};

#endif
