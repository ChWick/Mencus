#ifndef GRID2D_HPP
#define GRID2D_HPP

#include <vector>

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
    return (*this)[sizeY * x + y];
  }
  T &operator()(std::size_t x, std::size_t y) {
    return (*this)[sizeY * x + y];
  }
};

#endif
