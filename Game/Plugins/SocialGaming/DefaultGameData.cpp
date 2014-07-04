#include "DefaultGameData.hpp"

using namespace SocialGaming;

CDefaultGameData::CDefaultGameData() {
}
CDefaultGameData::~CDefaultGameData() {
}
CLevelList CDefaultGameData::getLevelList() {
  return m_LevelList;
}
void CDefaultGameData::setMissionStateOfLevel(EMissionState eMissionState, std::string sLevelName) {
  for (SLevelData &data : m_LevelList) {
    if (data.sLevelName == sLevelName) {
      data.eMissionState = eMissionState;
      return;
    }
  }

  SLevelData data;
  data.eMissionState = eMissionState;
  data.sLevelName = sLevelName;
  // add new
  m_LevelList.push_back(data);
}
