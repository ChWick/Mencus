#ifndef _DEBUG_TEXT_HPP_
#define _DEBUG_TEXT_HPP_

#include <OgrePrerequisites.h>
#if OGRE_VERSION >= ((1 << 16) | (9 << 8) | 0)
#include "OgreOverlaySystem.h"
#include "OgreOverlayManager.h"
#endif
#include "OgreTextAreaOverlayElement.h"
#include "OgreStringConverter.h"
 
 
class CDebugText
{
public:
    CDebugText()
    {
using namespace Ogre;
        olm=OverlayManager::getSingletonPtr();
        if(init==0)
        {
        panel=static_cast<OverlayContainer*>(olm->createOverlayElement("Panel","GUI"));
            panel->setMetricsMode(Ogre::GMM_PIXELS);
            panel->setPosition(0,0);
            panel->setDimensions(1.0f,1.0f);
            overlay=olm->create("GUI_OVERLAY");
            overlay->add2D(panel);
        }
        ++(this->init);
        szElement="element_"+StringConverter::toString(init);
        overlay=olm->getByName("GUI_OVERLAY");
        panel=static_cast<OverlayContainer*>(olm->getOverlayElement("GUI"));
        textArea=static_cast<TextAreaOverlayElement*>(olm->createOverlayElement("TextArea",szElement));
        panel->addChild(textArea);
        overlay->show();
    }
    ~CDebugText()
    {
szElement="element_"+Ogre::StringConverter::toString(init);
        olm->destroyOverlayElement(szElement);
        --(this->init);
        if(init==0)
        {
            olm->destroyOverlayElement("GUI");
            olm->destroy("GUI_OVERLAY");
        }
    }
    void setText(const char *szString)
    {
        textArea->setCaption(szString);
        textArea->setDimensions(1.0f,1.0f);
        textArea->setMetricsMode(Ogre::GMM_RELATIVE);
        textArea->setFontName("BlueHighway");
        textArea->setCharHeight(0.03f);
    }
  void setText(const Ogre::String &szString) // now You can use Ogre::String as text
    {
        textArea->setCaption(szString);
        textArea->setDimensions(1.0f,1.0f);
        textArea->setMetricsMode(Ogre::GMM_RELATIVE);
        textArea->setFontName("BlueHighway");
        textArea->setCharHeight(0.03f);
    }
    void setPos(float x,float y)
    {
        textArea->setPosition(x,y);
    }
  void setPosition(const Ogre::Vector2 &vPosition) {
    textArea->setPosition(vPosition.x, vPosition.y);
  }
    void setCol(float R,float G,float B,float I)
    {
        textArea->setColour(Ogre::ColourValue(R,G,B,I));
    }
  void setVisible(bool bVisible) {
    if (bVisible)
      textArea->show();
    else
      textArea->hide();
  }
  Ogre::TextAreaOverlayElement *getTextArea() {
    return textArea;
  }
  void scale(Ogre::Real scale) {
    textArea->setCharHeight(0.03f * scale);
  }
private:
Ogre::OverlayManager *olm;
Ogre::OverlayContainer *panel ;
Ogre::Overlay *overlay;
Ogre::TextAreaOverlayElement *textArea;
    static int init;
Ogre::String szElement;
};

#endif
