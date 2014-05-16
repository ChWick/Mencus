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
