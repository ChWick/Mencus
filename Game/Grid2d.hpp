#ifndef GRID2D_HPP
#define GRID2D_HPP

#include <vector>
#include <assert.h>

template <class T>
class grid2d : public std::vector<T> {
private:
  std::size_t sizeX;
  std::size_t sizeY;

public:

  std::size_t getSizeX() const {return sizeX;}
  std::size_t getSizeY() const {return sizeY;}

  void resize(std::size_t x, std::size_t y) {
    sizeX = x;
    sizeY = y;
    std::vector<T>::resize(x * y);
  }
  const T &operator()(std::size_t x, std::size_t y) const {
    assert(sizeY * x + y < std::vector<T>::size());
    return (*this)[sizeY * x + y];
  }
  T &operator()(std::size_t x, std::size_t y) {
    assert(sizeY * x + y < std::vector<T>::size());
    return (*this)[sizeY * x + y];
  }
};

#endif
