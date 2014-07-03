#ifndef _GUI_OVERLAY_HPP_
#define _GUI_OVERLAY_HPP_

class CGUIOverlay {
public:
  virtual ~CGUIOverlay() {}
  
  virtual void update(float tfp) {}

  virtual void changeTouchButtonSize(float fSize) {}
  virtual void onGUIScalingChanged(float fScaling) {}
private:
};

#endif
