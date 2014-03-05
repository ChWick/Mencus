#ifndef _ANDROID_INPUT_HPP_
#define _ANDROID_INPUT_HPP_

#include "OIS.h"
#include "../InputListener.hpp"

/*=============================================================================
  | Android input handling
  =============================================================================*/
class AndroidMultiTouch : public OIS::MultiTouch
{
public:
  AndroidMultiTouch():OIS::MultiTouch("Ogre", false, 0, 0){}
        
  /** @copydoc Object::setBuffered */
  virtual void setBuffered(bool buffered){}
        
  /** @copydoc Object::capture */
  virtual void capture(){}
        
  /** @copydoc Object::queryInterface */
  virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}
        
  /** @copydoc Object::_initialize */
  virtual void _initialize(){}
        
  OIS::MultiTouchState &getMultiTouchState(int i){
    while(i >= mStates.size()){
      Ogre::RenderWindow* pRenderWnd = static_cast<Ogre::RenderWindow*>(Ogre::Root::getSingleton().getRenderTarget("OgreWindow"));
      if(pRenderWnd)
	{
	  OIS::MultiTouchState state;
	  state.width = pRenderWnd->getWidth();
	  state.height = pRenderWnd->getHeight();
	  mStates.push_back(state);
	}
    }
    return mStates[i];
  }
};
    
class AndroidKeyboard : public OIS::Keyboard
{
public:
  AndroidKeyboard():OIS::Keyboard("Ogre", false, 1, 0){}
        
  /** @copydoc Object::setBuffered */
  virtual void setBuffered(bool buffered){}
        
  /** @copydoc Object::capture */
  virtual void capture(){}
        
  /** @copydoc Object::queryInterface */
  virtual OIS::Interface* queryInterface(OIS::Interface::IType type) {return 0;}
        
  /** @copydoc Object::_initialize */
  virtual void _initialize(){}
        
  virtual bool isKeyDown( OIS::KeyCode key ) const{
    return false;
  }
        
  virtual const std::string& getAsString( OIS::KeyCode kc ){
    static std::string defstr = "";
    return defstr;
  }
        
  virtual void copyKeyStates( char keys[256] ) const{
            
  }
};
    
/*=============================================================================
  | Android input injection
  =============================================================================*/
class AndroidInputInjector
{
private:
  CInputListenerManager* mInputListenerManager;
  AndroidMultiTouch* mTouch;
  AndroidKeyboard* mKeyboard;
        
public:
        
  AndroidInputInjector(CInputListenerManager* manager, AndroidMultiTouch* touch, AndroidKeyboard* keyboard) 
    : mInputListenerManager(manager), mTouch(touch), mKeyboard(keyboard) {}
        
  void injectKeyEvent(int action, int32_t keyCode)
  {
    if(keyCode == AKEYCODE_BACK)
      {
	OIS::KeyEvent evt(mKeyboard, OIS::KC_ESCAPE, 0);
	if(action == 0)
	  {
	    mInputListenerManager->keyPressed(evt);
	  }
	else
	  {
	    mInputListenerManager->keyReleased(evt);
	  }
      }
  }
        
  void injectTouchEvent(int action, float x, float y, int pointerId = 0)
  {
    OIS::MultiTouchState &state = mTouch->getMultiTouchState(pointerId);
            
    switch(action)
      {
      case 0:
	state.touchType = OIS::MT_Pressed;
	break;
      case 1:
	state.touchType = OIS::MT_Released;
	break;
      case 2:
	state.touchType = OIS::MT_Moved;
	break;
      case 3:
	state.touchType = OIS::MT_Cancelled;
	break;
      default:
	state.touchType = OIS::MT_None;
      }
            
    if(state.touchType != OIS::MT_None)
      {
	int last = state.X.abs;
	state.X.abs =  (int)x;
	state.X.rel = state.X.abs - last;
                
	last = state.Y.abs;
	state.Y.abs = (int)y;
	state.Y.rel = state.Y.abs - last;
                
	state.Z.abs = 0;
	state.Z.rel = 0;
                
	OIS::MultiTouchEvent evt(mTouch, state);
                
	switch(state.touchType)
	  {
	  case OIS::MT_Pressed:
	    mInputListenerManager->touchPressed(evt);
	    break;
	  case OIS::MT_Released:
	    mInputListenerManager->touchReleased(evt);
	    break;
	  case OIS::MT_Moved:
	    mInputListenerManager->touchMoved(evt);
	    break;
	  case OIS::MT_Cancelled:
	    mInputListenerManager->touchCancelled(evt);
	    break;
	  default:
	    break;
	  }
      }
  }
};

#endif