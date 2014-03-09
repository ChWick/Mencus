#include "GUIInstructions.hpp"


template<> CGUIInstructions *Ogre::Singleton<CGUIInstructions>::msSingleton = 0;

CGUIInstructions *CGUIInstructions::getSingletonPtr() {
  return msSingleton;
}
CGUIInstructions &CGUIInstructions::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CGUIInstructions::CGUIInstructions(CEGUI::Window *pGUIRoot) {
  using namespace CEGUI;

  ImageManager::getSingleton().loadImageset("instr.imageset");

  m_pInstructionsRoot = pGUIRoot->createChild("OgreTray/StaticImage", "Instructions/Root");
  m_pInstructionsRoot->setInheritsAlpha(false);
  m_pInstructionsRoot->setAlpha(1);
  m_pInstructionsRoot->setSize(USize(UDim(1, 0), UDim(1, 0)));
  m_pInstructionsRoot->setProperty("Image", "instructions/full");
  m_pInstructionsRoot->setProperty("FrameEnabled","False");
  m_pInstructionsRoot->setProperty("BackgroundEnabled","False");

  Window *pInsTitle = m_pInstructionsRoot->createChild("OgreTray/StaticText", "Instructions/Root/Text");
  pInsTitle->setPosition(UVector2(UDim(0.35, 0), UDim(0.13, 0)));
  pInsTitle->setSize(USize(UDim(0.4, 0), UDim(0.1, 0)));
  pInsTitle->setProperty("FrameEnabled","False");
  pInsTitle->setProperty("BackgroundEnabled","False");
  pInsTitle->setText("Anweisungen:");
  pInsTitle->setFont("diploma20");

  m_pText = m_pInstructionsRoot->createChild("OgreTray/StaticText", "Instructions/Root/Text");
  m_pText->setPosition(UVector2(UDim(0.3, 0), UDim(0.17, 0)));
  m_pText->setSize(USize(UDim(0.4, 0), UDim(0.6, 0)));
  m_pText->setProperty("FrameEnabled","False");
  m_pText->setProperty("BackgroundEnabled","False");
  m_pText->setFont("diploma15");

  m_pPressKeyText = m_pInstructionsRoot->createChild("OgreTray/StaticText", "Instructions/Root/Text");
  m_pPressKeyText->setPosition(UVector2(UDim(0.35, 0), UDim(0.9, 0)));
  m_pPressKeyText->setSize(USize(UDim(0.4, 0), UDim(0.1, 0)));
  m_pPressKeyText->setProperty("FrameEnabled","False");
  m_pPressKeyText->setProperty("BackgroundEnabled","False");
  m_pPressKeyText->setProperty("HorzFormatting", "HorzCentred");
  m_pPressKeyText->setText("- beliebige Taste zum Fortfahren -");

  CInputListenerManager::getSingleton().addInputListener(this);

  hide();
}
CGUIInstructions::~CGUIInstructions() {
  CInputListenerManager::getSingleton().removeInputListener(this);
}
bool CGUIInstructions::keyPressed( const OIS::KeyEvent &arg ) {
  m_pScreenplayListener->keyForContinueInstructionsPressed();

  return true;
}
bool CGUIInstructions::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) {
  m_pScreenplayListener->keyForContinueInstructionsPressed();

  return true;
}
bool CGUIInstructions::touchPressed(const OIS::MultiTouchEvent& evt) {
  m_pScreenplayListener->keyForContinueInstructionsPressed();

  return true;
}
