#include "GUITutorial.hpp"

using namespace CEGUI;


template<> CGUITutorial *Ogre::Singleton<CGUITutorial>::msSingleton = 0;

CGUITutorial *CGUITutorial::getSingletonPtr() {
  return msSingleton;
}
CGUITutorial &CGUITutorial::getSingleton() {
  assert(msSingleton);
  return *msSingleton;
}

CGUITutorial::CGUITutorial(Window *pRoot) 
  : m_pRoot(pRoot) {
}
CEGUI::Window *CGUITutorial::createMessageBox(const CEGUI::String &label) {
  FrameWindow *pWnd = dynamic_cast<FrameWindow*>(m_pRoot->createChild("OgreTray/FrameWindow", label));
  pWnd->setCloseButtonEnabled(true);
  pWnd->setRollupEnabled(false);
  pWnd->getTitlebar()->setFont("dejavusans12");
  pWnd->moveToFront();
  pWnd->setFont("dejavusans12");
  pWnd->setPosition(UVector2(UDim(0.2, 0), UDim(0.2, 0)));
  pWnd->setSize(USize(UDim(0.6, 0), UDim(0.6, 0)));
  pWnd->setText("Information");

  Window *pCloseBtn = pWnd->createChild("OgreTray/Button", "CloseButton");
  pCloseBtn->setUserData(pWnd);
  pCloseBtn->setPosition(UVector2(UDim(0.6, 0), UDim(0.85, 0)));
  pCloseBtn->setSize(USize(UDim(0.35, 0), UDim(0.1, 0)));
  pCloseBtn->setFont("dejavusans12");
  pCloseBtn->setText("Close");
  pCloseBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGUITutorial::onCloseButtonClicked, this));

  Window *pContent = pWnd->createChild("DefaultWindow", "Content");
  pContent->setPosition(UVector2(UDim(0.05, 0), UDim(0.05, 0)));
  pContent->setSize(USize(UDim(0.9, 0), UDim(0.75, 0)));

  return pContent;  
}
void CGUITutorial::showMoveInformationWindow() {
  showSimpleTextMessageBox("MoveInformationWindow", "Use the left arrow touch buttons to move Mencus left and right. Touch the button with the up arrow on the right hand side to jump.");
}
void CGUITutorial::showSwitchInformationWindow() {
  showSimpleTextMessageBox("SwitchInformationWindow",
			   "Oh, look! There is a switch! Use the switch-button on the right hand side and see what happens!\n\n"
			   "There are two types of switches:\n"
			   " - reusable ones, that will revert their actions\n"
			   " - single usable ones, that can't be used twice\n");
}
void CGUITutorial::showEnemyInformationWindow() {
  showSimpleTextMessageBox("EnemyInformationWindow",
			   "Watch out! An enemy! Use your bolt to kill him!\n\n"
			   "Your default weapon is the bolt. Hold the blot button on the right hand side to attack the enemy.\n\n"
			   "You can also change your weapons by pulling down the menu bar from the top.");
}
void CGUITutorial::showSmallEnemyInformationWindow() {
  showSimpleTextMessageBox("SmallEnemy",
			   "Care, this is a very small enemy, you cant hit him with your bolt usually.\n"
			   "In order to kill the enemy select the freezing bolt from the pull down menu to freeze him."
			   "When he is hit, you can easily kill the enemy with your bolt");
}
void CGUITutorial::showSimpleTextMessageBox(const CEGUI::String &label, const CEGUI::String &content) {
  pause(PAUSE_MAP_UPDATE);
  Window *pContent = createMessageBox(label);
  Window *pText = pContent->createChild("OgreTray/StaticText", "Text");
  pText->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pText->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pText->setFont("dejavusans12");
  pText->setText(content);
}
bool CGUITutorial::onCloseButtonClicked(const CEGUI::EventArgs &args) {
  Window *pBtn = dynamic_cast<const WindowEventArgs*>(&args)->window;
  FrameWindow *pWnd = static_cast<FrameWindow*>(pBtn->getUserData());
  pWnd->destroy();
  unpause(PAUSE_MAP_UPDATE);
  return true;
}
