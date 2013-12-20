#ifndef GRID2D_HPP
#define GRID2D_HPP

#include <vector>

template <class T>
class grid2d : public vector<T> {
private:
  size_t sizeX;
  size_t sizeY;

public:
  
  size_t getSizeX() const {return sizeX;}
  size_t getSizeY() const {return sizeY;}

  void resize(size_t x, size_t y) {
    sizeX = x;
    sizeY = y;
    vector<T>::resize(x * y);
  }
  const T &operator()(size_t x, size_t y) const {
    return (*this)[sizeY * x + y];
  }
  T &operator()(size_t x, size_t y) {
    return (*this)[sizeY * x + y];
  }
};

#endif
