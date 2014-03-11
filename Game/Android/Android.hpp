/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/
 
 Copyright (c) 2000-2013 Torus Knot Software Ltd
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */

#ifndef _ANDROID_HPP_
#define _ANDROID_HPP_


#ifdef INCLUDE_RTSHADER_SYSTEM
#   include "OgreRTShaderSystem.h"
#endif

#ifdef OGRE_STATIC_LIB
#  ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#    undef OGRE_STATIC_GLES
#    define INCLUDE_RTSHADER_SYSTEM
#    define OGRE_STATIC_GLES2
#  endif
#else
#error "Ogre has to be static!"
#endif

#  ifdef OGRE_BUILD_PLUGIN_CG
#  define OGRE_STATIC_CgProgramManager
#  endif

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
#   error This header is for use with Android only
#endif


#include <android_native_app_glue.h>
#include <android/log.h>
#include <EGL/egl.h>
#include "OgrePlatform.h"
#include "Android/OgreAndroidEGLWindow.h"
#include "../Game.hpp"
#include <stdio.h>

#ifdef OGRE_STATIC_LIB
#  include "OgreStaticPluginLoader.h"
#endif

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Ogre", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Ogre", __VA_ARGS__))

#include "AndroidInput.hpp"

class OgreAndroidBridge;
    
    
/*=============================================================================
  | Ogre Android bridge
  =============================================================================*/
class OgreAndroidBridge {
private:
  static bool m_bRenderPaused;
public:
  static void init(struct android_app* state) {
    state->onAppCmd = &OgreAndroidBridge::handleCmd;
    state->onInputEvent = &OgreAndroidBridge::handleInput;
            
    if(mInit)
      return;
    
    LOGI("Initialising Root");
    mRoot = new Ogre::Root();    
#ifdef OGRE_STATIC_LIB
    LOGI("Loading plugins");
    mStaticPluginLoader = new Ogre::StaticPluginLoader();
    mStaticPluginLoader->load();
    LOGI("plugins loaded");
#endif
    mRoot->setRenderSystem(mRoot->getAvailableRenderers().at(0));
    mRoot->initialise(false);
    mInit = true;
  }
        
  static void shutdown()
  {
    if(!mInit)
      return;
                
    mInit = false;
            
    if(mGame)
      {
	mGame->closeApp();
	OGRE_DELETE mGame;
	mGame = NULL;
      }
      
    OGRE_DELETE mRoot;
    mRoot = NULL;
    mRenderWnd = NULL;
            
    delete mTouch;
    mTouch = NULL;
            
    delete mKeyboard;
    mKeyboard = NULL;
            
    delete mInputInjector;
    mInputInjector = NULL;
            
#ifdef OGRE_STATIC_LIB
    mStaticPluginLoader->unload();
    delete mStaticPluginLoader;
    mStaticPluginLoader = NULL;
#endif
  }
        
  static int32_t handleInput(struct android_app* app, AInputEvent* event) 
  {
    if (mInputInjector)
      {
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) 
	  {
	    mTouch->clearStates();
	    for (size_t pi = 0; pi < AMotionEvent_getPointerCount(event); pi++) {
	      // LOGI("Count %d", AMotionEvent_getPointerCount(event));
	      int action = (int)(AMOTION_EVENT_ACTION_MASK & AMotionEvent_getAction(event));
	      
	      if(action == 0)
		mInputInjector->
		  injectTouchEvent(2,
				   AMotionEvent_getRawX(event, pi),
				   AMotionEvent_getRawY(event, pi), pi );
                    
	      mInputInjector->
		injectTouchEvent(action,
				 AMotionEvent_getRawX(event, pi),
				 AMotionEvent_getRawY(event, pi), pi);
	      //LOGI("Action of %d: %d", pi, action);
	    }
	    //LOGI("Pointer Count: %d  Number of states: %d", AMotionEvent_getPointerCount(event), mTouch->getMultiTouchStates().size());
	  }
	else 
	  {
	    mInputInjector->injectKeyEvent(AKeyEvent_getAction(event), AKeyEvent_getKeyCode(event));
	  }

	return 1;
      }
    return 0;
  }
        
  static void handleCmd(struct android_app* app, int32_t cmd)
  {
    switch (cmd) 
      {
      case APP_CMD_SAVE_STATE:
	break;
      case APP_CMD_INIT_WINDOW:
	m_bRenderPaused = false;
	LOGI("Initialising window command");
	if (app->window && mRoot) {
	  LOGI("... creating config");
	  AConfiguration* config = AConfiguration_new();
	  AConfiguration_fromAssetManager(config, app->activity->assetManager);
                        
	  if (!mRenderWnd) {
	    LOGI("... creating render window");
	    Ogre::NameValuePairList opt;
	    opt["externalWindowHandle"] = Ogre::StringConverter::toString((int)app->window);
	    opt["androidConfig"] = Ogre::StringConverter::toString((int)config);
                            
	    mRenderWnd = Ogre::Root::getSingleton().createRenderWindow("OgreWindow", 0, 0, false, &opt);
                            
	    if(!mTouch)
	      mTouch = new AndroidMultiTouch();
                            
	    if(!mKeyboard)
	      mKeyboard = new AndroidKeyboard();
                            
	    if(!mGame) {
	      LOGI("... creating game");
	      mGame = OGRE_NEW CGame();
	      mGame->initAppForAndroid(mRenderWnd, app, mTouch, mKeyboard);
	      mGame->initApp();
	      
	      mInputInjector = new AndroidInputInjector(CInputListenerManager::getSingletonPtr(), mTouch, mKeyboard);
	    }
	  }
	  else {
	    LOGI("... recreating render winow");
	    static_cast<Ogre::AndroidEGLWindow*>(mRenderWnd)->_createInternalResources(app->window, config);
	    if (mGame)
	      mGame->createResources();
	  }
                        
	  AConfiguration_delete(config);
	}
	break;
      case APP_CMD_TERM_WINDOW:
	if (mGame)
	  mGame->destroyResources();

	if(mRoot && mRenderWnd)
	  static_cast<Ogre::AndroidEGLWindow*>(mRenderWnd)->_destroyInternalResources();
	break;
      case APP_CMD_PAUSE:
	m_bRenderPaused = true;
	break;
      case APP_CMD_GAINED_FOCUS:
	break;
      case APP_CMD_LOST_FOCUS:
	break;
      case APP_CMD_CONFIG_CHANGED:
	break;
      }
  }
        
  static void go(struct android_app* state)
  {
    int ident, events;
    struct android_poll_source* source;
            
    while (true)
      {
	while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
	  {
	    if (source != NULL)
	      source->process(state, source);
                    
	    if (state->destroyRequested != 0)
	      return;
	  }
                
	if(mRenderWnd != NULL && mRenderWnd->isActive() && !m_bRenderPaused)
	  {
	    try {
	      mRenderWnd->windowMovedOrResized();
	      mRoot->renderOneFrame();
	    }
	    catch (const Ogre::Exception &e) {
	      LOGW("%s", e.getFullDescription().c_str());
	      break;
	    }
	    catch (...) {
	      LOGW("Unknown Exception");
	      break;
	    }
	  }
      }
  }
        
  static Ogre::RenderWindow* getRenderWindow()
  {
    return mRenderWnd;
  }
            
private:
  static CGame* mGame;
  static AndroidInputInjector* mInputInjector;
  static AndroidMultiTouch* mTouch;
  static AndroidKeyboard* mKeyboard;
  static Ogre::RenderWindow* mRenderWnd;
  static Ogre::Root* mRoot;
  static bool mInit;
        
#ifdef OGRE_STATIC_LIB
  static Ogre::StaticPluginLoader* mStaticPluginLoader;
#endif
};

#endif
