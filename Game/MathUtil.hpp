#ifndef _MATH_UTIL_HPP_
#define _MATH_UTIL_HPP_

#include <OgreVector2.h>

namespace MathUtil {
  template <class T>
  T linearInterpolate(const T &low, const T &high, double fac) {
    return low + (high - low) * fac;
  }
}

#endif
