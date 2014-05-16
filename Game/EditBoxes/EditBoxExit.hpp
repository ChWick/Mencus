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

#ifndef _EDIT_BOX_EXIT_
#define _EDIT_BOX_EXIT_

#include "EditBoxBase.hpp"
#include "Exit.hpp"

class CEditBoxExit : public CEditBoxBase {
private:
  CExit &m_Exit;
  CExit m_BackupExit;
  CEGUI::Combobox *m_pCombobox;
  CEGUI::Window *m_pContentId;
  CEGUI::Window *m_pContentRegion;
public:
  CEditBoxExit(int id,
	       CEGUI::Window *pParent,
	       float fButtonSize,
	       const CEGUI::String &sTitle,
	       CExit &exit) 
    : CEditBoxBase(id, pParent, fButtonSize, sTitle),
      m_Exit(exit),
      m_BackupExit(exit) {

    using namespace CEGUI;
    Combobox *pComboBox = dynamic_cast<Combobox*>(m_pWindow->createChild("OgreTray/Combobox", "ExitSelect"));
    m_pCombobox = pComboBox;
    pComboBox->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pComboBox->setSize(USize(UDim(1, 0), UDim(0, 3 * fButtonSize)));
    pComboBox->addItem(new ListboxTextItem("Region"));
    pComboBox->addItem(new ListboxTextItem("Enemy death"));
    pComboBox->setAutoSizeListHeightToContent(true);
    pComboBox->setReadOnly(true);
    pComboBox->subscribeEvent(Combobox::EventListSelectionAccepted,
			      Event::Subscriber(&CEditBoxExit::onListSelectionChanged, this));

    // id
    m_pContentId = m_pWindow->createChild("DefaultWindow", "id");
    m_pContentId->setPosition(UVector2(UDim(0, 0), UDim(0, fButtonSize)));
    m_pContentId->setSize(USize(UDim(1, 0), UDim(0, 2 * fButtonSize)));

    Window *pLabel = m_pContentId->createChild("OgreTray/Label", "Exitidlabel");
    pLabel->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    pLabel->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
    pLabel->setText("id");

    Window *pEditBox = m_pContentId->createChild("OgreTray/Editbox", "Exitid");
    pEditBox->setPosition(UVector2(UDim(0, fButtonSize), UDim(0, 0)));
    pEditBox->setSize(USize(UDim(1, -fButtonSize), UDim(0, fButtonSize)));
    pEditBox->setText(m_Exit.getID());
    
    m_pContentId->setVisible(m_Exit.getExitType() == EXIT_ENEMY_DEATH);

    // region
    m_pContentRegion = m_pWindow->createChild("DefaultWindow", "region");
    m_pContentRegion->setPosition(UVector2(UDim(0, 0), UDim(0, fButtonSize)));
    m_pContentRegion->setSize(USize(UDim(1, 0), UDim(0, 2 * fButtonSize)));

    createVector(m_pContentRegion, "pos", m_Exit.getBoundingBox().getPosition().x, m_Exit.getBoundingBox().getPosition().y, fButtonSize, 0);
    createVector(m_pContentRegion, "size", m_Exit.getBoundingBox().getSize().x, m_Exit.getBoundingBox().getSize().y, fButtonSize, fButtonSize);

    m_pContentRegion->setVisible(m_Exit.getExitType() == EXIT_REGION);
    

    // default selected exit type
    m_pCombobox->setItemSelectState(m_Exit.getExitType(), true);
  }
protected:
  void createVector(CEGUI::Window *pParent, const CEGUI::String &label, float fDefaultValueX, float fDefaultValueY, float fButtonSize, float fPosY) {
    using namespace CEGUI;
    Window *pLabel = pParent->createChild("OgreTray/Label", label + "label");
    pLabel->setPosition(UVector2(UDim(0, 0), UDim(0, fPosY)));
    pLabel->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
    pLabel->setText(label);

    createSpinner(pParent, label + "x", fDefaultValueX, fButtonSize, fPosY, false);
    createSpinner(pParent, label + "y", fDefaultValueY, fButtonSize, fPosY, true);
  }
  CEGUI::Spinner *createSpinner(CEGUI::Window *pParent, const CEGUI::String &label, float fDefaultValue, float fButtonSize, float fPosY, bool bIsY) {
    using namespace CEGUI;
    Spinner *pSpinner = dynamic_cast<Spinner*>(pParent->
					       createChild("OgreTray/Spinner", label));
    pSpinner->setTextInputMode(Spinner::FloatingPoint);
    if (!bIsY) 
      pSpinner->setPosition(UVector2(UDim(0, fButtonSize), UDim(0, fPosY)));
    else
      pSpinner->setPosition(UVector2(UDim(0.5, 0.5 * fButtonSize), UDim(0, fPosY)));

    pSpinner->setSize(USize(UDim(0.5, -0.5 * fButtonSize), UDim(0, fButtonSize)));
    pSpinner->setText(PropertyHelper<float>::toString(fDefaultValue));
    pSpinner->setMinimumValue(0);
    pSpinner->subscribeEvent(Spinner::EventValueChanged,
			     Event::Subscriber(&CEditBoxExit::onBoundingBoxSpinnerChanged, this));
    return pSpinner;
  }
  bool onBoundingBoxSpinnerChanged(const CEGUI::EventArgs &args) {
    using namespace CEGUI;
    m_Exit.setBoundingBox(CBoundingBox2d(Ogre::Vector2(dynamic_cast<Spinner*>(m_pContentRegion->getChild("posx"))->getCurrentValue(),
					dynamic_cast<Spinner*>(m_pContentRegion->getChild("posy"))->getCurrentValue()),
			  Ogre::Vector2(dynamic_cast<Spinner*>(m_pContentRegion->getChild("sizex"))->getCurrentValue(),
					dynamic_cast<Spinner*>(m_pContentRegion->getChild("sizey"))->getCurrentValue())));
    return true;
  }
  bool onListSelectionChanged(const CEGUI::EventArgs &args) {
    int id = -1;
    if (m_pCombobox->getSelectedItem()) {
      id = m_pCombobox->getItemIndex(m_pCombobox->getSelectedItem());
      m_Exit.setExitType(static_cast<EExitTypes>(id));
    }
    m_pContentId->setVisible(id == EXIT_ENEMY_DEATH);
    m_pContentRegion->setVisible(id == EXIT_REGION);
    return true;
  }
  bool accepted() {
    m_Exit.setID(m_pContentId->getChild("Exitid")->getText().c_str());
    return true;
  }
  bool canceled() {
    m_Exit = m_BackupExit;
    return true;
  }
};

#endif
