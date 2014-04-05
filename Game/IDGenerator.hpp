#ifndef _ID_GENERATOR_HPP_
#define _ID_GENERATOR_HPP_

#include <random>
#include <string>
#include <cstdlib>
#include <OgreStringConverter.h>

class CIDGenerator {
private:
  static std::default_random_engine m_generator;
  static std::uniform_int_distribution<long> m_distribution;
public:
  //! Simply return a random long long, should never generate the same number twice
  static long nextID() {
    return m_distribution(m_generator);
  }
  static std::string nextID(const std::string &prefix) {
    return prefix + Ogre::StringConverter::toString(nextID());
  }
};

#endif
