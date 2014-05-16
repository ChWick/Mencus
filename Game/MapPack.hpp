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

#ifndef _MAP_PACK_HPP_
#define _MAP_PACK_HPP_

#include <string>
#include "XMLResources/Manager.hpp"
#include <memory>

class CMapInfo;
class CMapPack;
typedef std::shared_ptr<const CMapPack> CMapPackConstPtr;
typedef std::shared_ptr<CMapPack> CMapPackPtr;

class CMapPack
  : public XMLResources::CManager {
  std::string m_sMapName;
  bool m_bInitialised;
  std::shared_ptr<CMapInfo> m_pMapInfo;
public:
  CMapPack(const std::string &sMapName);
  CMapPack(std::shared_ptr<CMapInfo> pMapInfo);
  ~CMapPack();

  void init();

  void setMapInfo(std::shared_ptr<CMapInfo> pInfo);

  const std::string &getMapName() const {return m_sMapName;}
  std::string generateInfoText() const;

  const std::shared_ptr<const CMapInfo> getMapInfo() const {return m_pMapInfo;}
  std::shared_ptr<CMapInfo> getMapInfo() {return m_pMapInfo;}
};


#endif
