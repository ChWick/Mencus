#ifndef _GUI_OVERLAY_HPP_
#define _GUI_OVERLAY_HPP_

class CGUIOverlay {
public:
  virtual ~CGUIOverlay() {}

  virtual void changeTouchButtonSize(float fSize) {}
private:
};

#endif
