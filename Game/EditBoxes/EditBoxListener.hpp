#ifndef _EDIT_BOX_LISTENER_
#define _EDIT_BOX_LISTENER_

class CEditBoxBase;

class CEditBoxListener {
public:
  virtual void onEditBoxAccept(CEditBoxBase *) = 0;
  virtual void onEditBoxCancel(CEditBoxBase *) = 0;
};

#endif
