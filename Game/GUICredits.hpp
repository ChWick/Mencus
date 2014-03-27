#ifndef _GUI_CREDITS_HPP_
#define _GUI_CREDITS_HPP_

#include <CEGUI/CEGUI.h>
#include "Settings.hpp"
#include "GameState.hpp"
#include <OgreResourceGroupManager.h>

class CGUICredits {
private:
  CEGUI::Window *m_pRoot;

public:
  CGUICredits() {    
    using namespace CEGUI;
    Window *pGlobalRoot = System::getSingleton().getDefaultGUIContext().getRootWindow();

    float fButtonSize = 0.1;

    m_pRoot = pGlobalRoot->createChild("OgreTray/Group", "Credits");
    m_pRoot->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    m_pRoot->setSize(USize(UDim(1, 0), UDim(1, 0)));
    m_pRoot->setText("Credits");
    
    MultiLineEditbox *pText = dynamic_cast<MultiLineEditbox*>(m_pRoot->createChild("OgreTray/MultiLineEditbox", "Text"));
    pText->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pText->setSize(USize(UDim(1, 0), UDim(1 - fButtonSize, 0)));
    pText->setReadOnly(true);
    pText->setText(Ogre::ResourceGroupManager::getSingleton().openResource("credits", "General")->getAsString().c_str());

    Window *pButton = m_pRoot->createChild("OgreTray/Button", "CloseButton");
    pButton->setPosition(UVector2(UDim(0.25f, 0), UDim(1 - fButtonSize, 0)));
    pButton->setSize(USize(UDim(0.5, 0), UDim(fButtonSize, 0)));
    pButton->setText("Close");
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CGUICredits::onClose, this));
  }
  ~CGUICredits() {
    m_pRoot->destroy();
  }

private:
  bool onClose(const CEGUI::EventArgs &args) {
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU);
    return true;
  }
};


#endif
