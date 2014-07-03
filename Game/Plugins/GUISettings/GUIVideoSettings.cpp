#include "GUIVideoSettings.hpp"
#include "Settings.hpp"
#include "GUIManager.hpp"

using namespace CEGUI;

CGUIVideoSettings::CGUIVideoSettings(CEGUI::Window *pParent) 
  : CGUISettingsWindow(pParent, "Video") {
  Window *pMenuSizeText = m_pContent->createChild("OgreTray/StaticText", "MenuSizeText");
  pMenuSizeText->setPosition(UVector2(UDim(0, 0), UDim(0.00, 0)));
  pMenuSizeText->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pMenuSizeText->setProperty("FrameEnabled", "False");
  pMenuSizeText->setProperty("BackgroundEnabled", "False");
  Slider * pMenuSizeSlider = dynamic_cast<Slider*>(m_pContent->createChild("OgreTray/Slider", "MenuSizeSlider"));
  pMenuSizeSlider->setPosition(UVector2(UDim(0, 0), UDim(0.12, 0)));
  pMenuSizeSlider->setSize(USize(UDim(1, 0), UDim(0.1, 0)));
  pMenuSizeSlider->setMaxValue(1);
  pMenuSizeSlider->setClickStep(0.1);
  pMenuSizeSlider->setCurrentValue(0.24152342); // to force call of event
  pMenuSizeSlider->subscribeEvent(Slider::EventValueChanged,
				    Event::Subscriber(&CGUIVideoSettings::menuSizeSliderValueChanged,
						      this));
  // initial value
  pMenuSizeSlider->setCurrentValue(CSettings::getSingleton().getVideoSettings().m_fHUDSize);
}

bool CGUIVideoSettings::menuSizeSliderValueChanged(const EventArgs &args) {
  const WindowEventArgs &wndArgs = dynamic_cast<const WindowEventArgs&>(args);
  const Slider *pSlider = dynamic_cast<const Slider*>(wndArgs.window);

  m_pContent->getChild("MenuSizeText")->setText("Main menu size: " + PropertyHelper<int>::toString(pSlider->getCurrentValue() * 100) + " %");

  CGUIManager::getSingleton().onGUIScalingChanged(pSlider->getCurrentValue());
  CSettings::getSingleton().getVideoSettings().m_fHUDSize = pSlider->getCurrentValue();

  return true;
}
