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
