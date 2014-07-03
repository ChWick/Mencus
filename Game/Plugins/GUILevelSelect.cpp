#include "GUILevelSelect.hpp"
#include "GUIManager.hpp"
#include "XMLResources/Manager.hpp"
#include "GameState.hpp"
#include "LevelState.hpp"
#include "MapPack.hpp"
#include "OgreLogManager.h"

using namespace CEGUI;

CGUILevelSelect::CGUILevelSelect(CEGUI::Window *pParent) {
  Window *pPane = pParent->createChild("OgreTray/StaticImage");
  m_pRoot = pPane;
  pPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pPane->setSize(USize(UDim(1, 0), UDim(1, 0)));
  pPane->setAlwaysOnTop(true);
  pPane->moveToFront();
  pPane->setProperty("FrameEnabled", "false");
  pPane->setProperty("Image", "OgreTrayImages/Shade");
  
  Window *pWnd = pPane->createChild("OgreTray/Group");
  pWnd->setPosition(UVector2(UDim(0.1, 0), UDim(0.1, 0)));
  pWnd->setSize(USize(UDim(0.8, 0), UDim(0.8, 0)));
  pWnd->setText(XMLResources::GLOBAL.getCEGUIString("select_level"));

  Window *pBackBtn = pWnd->createChild("OgreTray/Button", "BackButton");
  pBackBtn->setUserData(dynamic_cast<Window*>(pWnd)); // to be sure that is a Window * ptr
  pBackBtn->setPosition(UVector2(UDim(0.05, 0), UDim(0.80, 0)));
  pBackBtn->setSize(USize(UDim(0.35, 0), UDim(0.2, 0)));
  pBackBtn->setText(XMLResources::GLOBAL.getCEGUIString("back"));
  pBackBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGUILevelSelect::onBackButtonClicked, this));

  Window *pPlayBtn = pWnd->createChild("OgreTray/Button", "PlayButton");
  pPlayBtn->setUserData(dynamic_cast<Window*>(pWnd)); // to be sure that is a Window * ptr
  pPlayBtn->setPosition(UVector2(UDim(0.6, 0), UDim(0.80, 0)));
  pPlayBtn->setSize(USize(UDim(0.35, 0), UDim(0.2, 0)));
  pPlayBtn->setText(XMLResources::GLOBAL.getCEGUIString("start_game"));
  pPlayBtn->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CGUILevelSelect::onPlayButtonClicked, this));

  Window *pContent = pWnd->createChild("DefaultWindow", "Content");
  m_pContent = pContent;
  pContent->setPosition(UVector2(UDim(0.05, 0), UDim(0.05, 0)));
  pContent->setSize(USize(UDim(0.9, 0), UDim(0.75, 0)));

  ScrollablePane *pLevelPane = dynamic_cast<ScrollablePane*>(pContent->createChild("OgreTray/ScrollablePane", "Pane"));
  pLevelPane->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
  pLevelPane->setSize(USize(UDim(0.7, 0), UDim(1, 0)));

  MultiLineEditbox *pLevelInfoWindow = dynamic_cast<MultiLineEditbox*>(pContent->createChild("OgreTray/MultiLineEditbox", "Info"));
  pLevelInfoWindow->setPosition(UVector2(UDim(0.7, 0), UDim(0, 0)));
  pLevelInfoWindow->setSize(USize(UDim(0.3, 0), UDim(1, 0)));
  pLevelInfoWindow->setReadOnly(true);

  Window *pChickenButton = pWnd->createChild("OgreTray/ImageButton",
						     "ChickenButton");
  pChickenButton->setPosition(UVector2(UDim(0.45, 0), UDim(0.80, 0)));
  pChickenButton->setSize(USize(UDim(0.15, 0), UDim(0.2, 0)));
  pChickenButton->setText("0");
  pChickenButton->setProperty("Image", "hud_weapons/skip");
  pChickenButton->subscribeEvent(PushButton::EventClicked,
				 Event::Subscriber(&CGUILevelSelect::onChickenPressed, this));
  m_pChickenButton = pChickenButton;


  updateLevelsSelection();
}

CGUILevelSelect::~CGUILevelSelect() {
  m_pRoot->destroy();
}

bool CGUILevelSelect::onBackButtonClicked(const CEGUI::EventArgs &args) {
  CGUIManager::getSingleton().destroyGUIOverlay(this);
  return true;
}

bool CGUILevelSelect::onPlayButtonClicked(const CEGUI::EventArgs &args) {
  CMapPackPtr mapPack(new CMapPack(m_pLevelInfo->sLevelFileName));
  CGameState::getSingleton().changeGameState(CGameState::GS_GAME, mapPack);
  CGUIManager::getSingleton().destroyGUIOverlay(this);
  return true;
}

void CGUILevelSelect::updateLevelsSelection() {
  int iLeftChickens = 3; // maximum number of chickens

  ScrollablePane *pPane = dynamic_cast<ScrollablePane*>(m_pContent->getChild("Pane"));
  pPane->getHorzScrollbar()->setVisible(false);
  pPane->setShowHorzScrollbar(false);
  const Window *pContent = pPane->getContentPane();
  for (int i = pContent->getChildCount() - 1; i >= 0; i--) {
    pContent->getChildAtIdx(i)->destroy();
  }

  m_LevelList.load();
  const std::list<SLevelInfo> &lList(m_LevelList.getLevelInfoList());

  unsigned int uiLevelsPerRow = 5;
  float fButtonSize = pContent->getPixelSize().d_width / uiLevelsPerRow;
  std::list<SLevelInfo>::const_iterator it = lList.cbegin();
  std::string sPreviousLevelFileName;
  const SLevelInfo *pPreviousLevelInfo(NULL);
  RadioButton *pLastEnabled = NULL;
  for (unsigned int i = 0; i < lList.size(); i++) {
    RadioButton *pBut = dynamic_cast<RadioButton*>(pPane->createChild("OgreTray/ToggleRadioButton", PropertyHelper<unsigned int>::toString(i + 1)));
    pBut->setText(pBut->getName());
    pBut->setSize(USize(UDim(0, fButtonSize), UDim(0, fButtonSize)));
    pBut->setPosition(UVector2(UDim(0, fButtonSize * (i % uiLevelsPerRow)), UDim(0, fButtonSize * (i / uiLevelsPerRow))));
    pBut->setUserData(const_cast<SLevelInfo*>(&(*it)));
    //pBut->setProperty("HorzFormatting", "CenterAligned");
    pBut->subscribeEvent(RadioButton::EventSelectStateChanged,
			 Event::Subscriber(&CGUILevelSelect::onLevelButtonClicked,
					   this));
    pBut->setGroupID(1249845902);
    pBut->setSelected(false);
    if (i > 0) {
      if (CLevelState::has(it->sLevelFileName) &&
	  CLevelState::get(it->sLevelFileName).eMissionState == MS_SKIPPED) {
	pBut->setEnabled(true);
	pBut->setProperty("Image", "hud_weapons/skip");
	--iLeftChickens;
      }
      else {
	pBut->setProperty("Image", "");
	if (it->bTutorial) {
	  CLevelState::get(it->sLevelFileName, true);
	  pBut->setEnabled(true);
	}
	else if ((CLevelState::has(sPreviousLevelFileName) &&
		 CLevelState::get(sPreviousLevelFileName).eMissionState
		  != MS_FAILED) ||
		 (pPreviousLevelInfo && pPreviousLevelInfo->bTutorial)) {
	  pLastEnabled = pBut;
	  pBut->setEnabled(true);
	}
	else {
	  pBut->setEnabled(false);
	  pBut->setText("");
	  pBut->setProperty("Image", "hud_weapons/lock");
	}
      }
    }
    else {
      pLastEnabled = pBut;
    }
    if (it->bTutorial) {
      // Own image for tutorial
      pBut->setProperty("Image", "hud_weapons/tutorial");
    }
    sPreviousLevelFileName = it->sLevelFileName;
    pPreviousLevelInfo = &(*it);
    it++;
  }

  if (pLastEnabled) {
    pLastEnabled->setSelected(true);
  }


  m_pChickenButton->setText(PropertyHelper<int>::toString(iLeftChickens));
  m_pChickenButton->setEnabled(iLeftChickens > 0);
  //selectLevel(1);
}

void CGUILevelSelect::selectLevel(unsigned int id) {
  m_uiSelectedLevelID = id;
  ScrollablePane *pPane = dynamic_cast<ScrollablePane*>(m_pContent->getChild("Pane"));
  MultiLineEditbox *pInfoPane = dynamic_cast<MultiLineEditbox*>(m_pContent->getChild("Info"));
  pPane->setShowHorzScrollbar(false);
  Window *pBut = pPane->getChild(PropertyHelper<unsigned int>::toString(id));

  SLevelInfo *pLevelInfo = static_cast<SLevelInfo*>(pBut->getUserData());

  // load map pack for receiving info
  CMapPack pack(pLevelInfo->sLevelFileName);
  pInfoPane->setText(reinterpret_cast<const utf8*>(pack.generateInfoText().c_str()));

  m_pLevelInfo = pLevelInfo;
}
bool CGUILevelSelect::onLevelButtonClicked(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs(dynamic_cast<const WindowEventArgs &>(args));
  Ogre::LogManager::getSingleton().logMessage(Ogre::String("Level ") + wndArgs.window->getName().c_str() + Ogre::String(" selected"));
  selectLevel(PropertyHelper<unsigned int>::fromString(wndArgs.window->getName()));

  return true;
}
bool CGUILevelSelect::onChickenPressed(const CEGUI::EventArgs &args) {
  const WindowEventArgs &wndArgs(dynamic_cast<const WindowEventArgs &>(args));
  unsigned int uiSelectedLevelID = m_uiSelectedLevelID;
  if (PropertyHelper<int>::fromString(wndArgs.window->getText()) <= 0) {
    // no more chicken available
    return true;
  }

  ScrollablePane *pPane = dynamic_cast<ScrollablePane*>(m_pContent->getChild("Pane"));
  Window *pBut = pPane->getChild(PropertyHelper<unsigned int>::toString(m_uiSelectedLevelID));

  SLevelInfo *pLevelInfo = static_cast<SLevelInfo*>(pBut->getUserData());
  SStatistics &stats(CLevelState::get(pLevelInfo->sLevelFileName, true));
  if (stats.eMissionState == MS_FAILED && !pLevelInfo->bTutorial) {
    stats.eMissionState = MS_SKIPPED;
    CLevelState::add(stats);
  }
  else {
    return true;
  }

  // update levels
  updateLevelsSelection();
  try {
    RadioButton *pRadio = dynamic_cast<RadioButton*>(pPane->getChild(PropertyHelper<unsigned int>::toString(uiSelectedLevelID + 1)));
    if (pRadio) {
      pRadio->setSelected(true);
    }
  }
  catch (...) {
    // child not found, end of map
  }
  return true;
}
