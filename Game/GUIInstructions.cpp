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
  m_pInstructionsRoot->setProperty("Image", "instructions/full_image");
  m_pInstructionsRoot->setProperty("FrameEnabled","False");
  m_pInstructionsRoot->setProperty("BackgroundEnabled","False");

  m_pText = m_pInstructionsRoot->createChild("OgreTray/StaticText", "Instructions/Root/Text");
  m_pText->setPosition(UVector2(UDim(0.3, 0), UDim(0.3, 0)));
  m_pText->setSize(USize(UDim(0.4, 0), UDim(0.4, 0)));
  m_pText->setProperty("FrameEnabled","False");
  m_pText->setProperty("BackgroundEnabled","False");


  hide();
}
