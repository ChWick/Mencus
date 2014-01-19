#include "GUIGameOver.hpp"


template<> CGUIGameOver *Ogre::Singleton<CGUIGameOver>::msSingleton = 0;

CGUIGameOver *CGUIGameOver::getSingletonPtr() {
  return msSingleton;
}
CGUIGameOver &CGUIGameOver::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CGUIGameOver::CGUIGameOver(CEGUI::Window *pGUIRoot) {
  using namespace CEGUI;

  ImageManager::getSingleton().loadImageset("game_over.imageset");

  m_pRoot = pGUIRoot->createChild("OgreTray/StaticImage", "Instructions/Root");
  m_pRoot->setInheritsAlpha(false);
  m_pRoot->setAlpha(1);
  m_pRoot->setSize(USize(UDim(1, 0), UDim(1, 0)));
  m_pRoot->setProperty("Image", "game_over/full");
  m_pRoot->setProperty("FrameEnabled","False");
  m_pRoot->setProperty("BackgroundEnabled","False");

  CInputListenerManager::getSingleton().addInputListener(this);

  hide();
}
CGUIGameOver::~CGUIGameOver() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
void CGUIGameOver::update(Ogre::Real tpf) {
}
bool CGUIGameOver::keyPressed( const OIS::KeyEvent &arg ) {
  return true;
}
