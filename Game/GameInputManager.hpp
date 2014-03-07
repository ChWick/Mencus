#ifndef GAME_INPUT_MANAGER_HPP
#define GAME_INPUT_MANAGER_HPP

#include "InputListener.hpp"
#include <OgreSingleton.h>
#include <list>

class CGameInputListener;
class CGameInputCommand;

class CGameInputManager :
  public CInputListener,
  public Ogre::Singleton<CGameInputManager> {
private:
  std::list<CGameInputListener*> m_lListenerList;
public:
  static CGameInputManager& getSingleton(void);
  static CGameInputManager* getSingletonPtr(void);

  CGameInputManager();
  ~CGameInputManager();

  void addInputListener(CGameInputListener *pListener) {
    m_lListenerList.push_back(pListener);
  }
  void removeInputListener(CGameInputListener *pListener) {
    m_lListenerList.remove(pListener);
  }

  // OIS::KeyListener
  virtual bool keyPressed( const OIS::KeyEvent &arg );
  virtual bool keyReleased( const OIS::KeyEvent &arg );
  // OIS::MouseListener
  virtual bool mouseMoved( const OIS::MouseEvent &arg );
  virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
  // OIS::MultiTouchListener
  virtual bool touchMoved(const OIS::MultiTouchEvent& evt);
  virtual bool touchPressed(const OIS::MultiTouchEvent& evt);
  virtual bool touchReleased(const OIS::MultiTouchEvent& evt);
  virtual bool touchCancelled(const OIS::MultiTouchEvent& evt);

private:
  void sendCommandToListeners( const CGameInputCommand &cmd );

};


#endif
