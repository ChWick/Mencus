/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

#ifndef _GUI_CREDITS_HPP_
#define _GUI_CREDITS_HPP_

#include <CEGUI/CEGUI.h>
#include "Settings.hpp"
#include "GameState.hpp"
#include "PauseCaller.hpp"
#include <OgreResourceGroupManager.h>
#include "XMLResources/Manager.hpp"

class CGUICredits
  : public CPauseCaller {
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
    m_pRoot->setText(XMLResources::GLOBAL.getCEGUIString("credits"));
    
    MultiLineEditbox *pText = dynamic_cast<MultiLineEditbox*>(m_pRoot->createChild("OgreTray/MultiLineEditbox", "Text"));
    pText->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pText->setSize(USize(UDim(1, 0), UDim(1 - fButtonSize, 0)));
    pText->setReadOnly(true);
    pText->setText(Ogre::ResourceGroupManager::getSingleton().openResource("credits", "General")->getAsString().c_str());
    pText->setTextParsingEnabled(true);

    Window *pButton = m_pRoot->createChild("OgreTray/Button", "CloseButton");
    pButton->setPosition(UVector2(UDim(0.25f, 0), UDim(1 - fButtonSize, 0)));
    pButton->setSize(USize(UDim(0.5, 0), UDim(fButtonSize, 0)));
    pButton->setText(XMLResources::GLOBAL.getCEGUIString("close"));
    pButton->subscribeEvent(PushButton::EventClicked,
			    Event::Subscriber(&CGUICredits::onClose, this));

    pause(PAUSE_ALL);
  }
  ~CGUICredits() {
    unpause(PAUSE_ALL);
    m_pRoot->destroy();
  }

private:
  bool onClose(const CEGUI::EventArgs &args) {
    CGameState::getSingleton().changeGameState(CGameState::GS_MAIN_MENU);
    return true;
  }
};


#endif
