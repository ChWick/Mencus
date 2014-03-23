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
  grid2d() 
    : sizeX(0), sizeY(0) {
  }
  grid2d(std::size_t x, std::size_t y) {
    resize(x, y);
  }
  grid2d(std::size_t x, std::size_t y, const T &value) {
    resize(x, y, value);
  }

  std::size_t getSizeX() const {return sizeX;}
  std::size_t getSizeY() const {return sizeY;}

  void resize(std::size_t x, std::size_t y) {
    sizeX = x;
    sizeY = y;
    std::vector<T>::resize(x * y);
  }
  void resize(std::size_t x, std::size_t y, const T &value) {
    sizeX = x;
    sizeY = y;
    std::vector<T>::resize(x * y, value);
  }
  const T &operator()(std::size_t x, std::size_t y) const {
    assert(sizeY * x + y < std::vector<T>::size());
    return (*this)[sizeY * x + y];
  }
  T &operator()(std::size_t x, std::size_t y) {
    assert(sizeY * x + y < std::vector<T>::size());
    return (*this)[sizeY * x + y];
  }

  grid2d<T> &operator=(const grid2d<T> &src) {
    std::vector<T>::operator=(src);
    sizeX = src.sizeX;
    sizeY = src.sizeY;
    return *this;
  }
};

#endif
