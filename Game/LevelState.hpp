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

#ifndef _LEVEL_STATE_HPP_
#define _LEVEL_STATE_HPP_

#include <string>
#include <vector>
#include "Statistics.hpp"
#include <tinyxml2.h>

class CLevelState {
private:
  static std::vector<SStatistics> m_vLevelStatistics;
  static bool m_bLoaded;
  static tinyxml2::XMLDocument m_XMLDocument;
public:
  static bool has(const std::string &sFileName);
  static SStatistics &get(const std::string &sFileName,
			  bool createIfNotExisting = false);
  static tinyxml2::XMLElement *getXMLElement(const std::string &sFileName);
  static bool levelAccomplished(const std::string &sFileName);
  static void add(const SStatistics &stats);

private:
  static void read();
  static void write();
};

#endif
