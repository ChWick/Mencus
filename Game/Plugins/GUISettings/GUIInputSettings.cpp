#include "GUIInputSettings.hpp"
#include "Settings.hpp"
#include "GUIManager.hpp"

using namespace CEGUI;

CGUIInputSettings::CGUIInputSettings(CEGUI::Window *pParent) 
  : CGUISettingsWindow(pParent, "Input") {
  float fHeight = 0;

#ifdef INPUT_TOUCH
  Window *pButtonSizeText = m_pContent->createChild("OgreTray/StaticText", "ButtonSliderText");
  pButtonSizeText->setPosition(UVector2(UDim(0, 0), UDim(fHeight, 0)));
  pButtonSizeText->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pButtonSizeText->setProperty("FrameEnabled", "False");
  pButtonSizeText->setProperty("BackgroundEnabled", "False");
  Slider * pButtonSizeSlider = dynamic_cast<Slider*>(m_pContent->createChild("OgreTray/Slider", "ButtonSizeSlider"));
  pButtonSizeSlider->setPosition(UVector2(UDim(0, 0), UDim(fHeight + 0.12, 0)));
  pButtonSizeSlider->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pButtonSizeSlider->setMaxValue(300);
  pButtonSizeSlider->setClickStep(20);
  pButtonSizeSlider->subscribeEvent(Slider::EventValueChanged,
				    Event::Subscriber(&CGUIInputSettings::buttonSizeSliderValueChanged,
						      this));
  // this will also set the initial value for the touch buttons
  pButtonSizeSlider->setCurrentValue(CSettings::getSingleton().getInputSettings().m_fTouchButtonSize);
  fHeight += 0.25;
#endif
}

bool CGUIInputSettings::buttonSizeSliderValueChanged(const EventArgs &args) {
#ifdef INPUT_TOUCH
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  const Slider *pSlider = dynamic_cast<const Slider*>(wndArgs.window);

  m_pContent->getChild("ButtonSliderText")->setText("Input touch button size: " + PropertyHelper<int>::toString(pSlider->getCurrentValue()) + " px");

  CGUIManager::getSingleton().changeTouchButtonSize(pSlider->getCurrentValue());
  CSettings::getSingleton().getInputSettings().m_fTouchButtonSize = pSlider->getCurrentValue();
#endif

  return true;
}
