#ifndef SCREENPLAYLISTENER_HPP
#define SCREENPLAYLISTENER_HPP

class CScreenplayListener {
public:
  virtual void keyForContinueInstructionsPressed() = 0;
  virtual void playerExitsMap() = 0;
};
#endif // SCREENPLAYLISTENER_HPP
