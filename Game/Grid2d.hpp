/*****************************************************************************
 * Copyright 2014 Christoph Wick
 *
 * This file is part of Mencus.
 *
 * Mencus is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Mencus is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Mencus. If not, see http://www.gnu.org/licenses/.
 *****************************************************************************/

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
