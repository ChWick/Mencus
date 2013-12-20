#ifndef COLLISION_DEFINES_HPP
#define COLLISION_DEFINES_HPP

//! Directions in that a collision occured
enum ECollisionCheckDirections {
  CCD_NONE    = 0,
  CCD_TOP     = 1,
  CCD_LEFT    = 2,
  CCD_BOTTOM  = 4,
  CCD_RIGHT   = 8,
};
enum ECollisionLineDirections {
  CLD_HORIZONTAL,
  CLD_VERTICAL,
};

#endif
