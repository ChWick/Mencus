#ifndef UPGRADE_BASE
#define UPGRADE_BASE


#include <tinyxml2.h>
#include <iostream>
#include "UpgradeResult.hpp"

using namespace tinyxml2;
using namespace std;

class UpgradeBase {
protected:
  int m_iFromVersion;
  int m_iToVersion;
  int m_iCounter;
public:
  UpgradeBase(int fromVersion, int toVersion) 
    : m_iFromVersion(fromVersion),
      m_iToVersion(toVersion),
      m_iCounter(0) {
  }
  EUpgradeResult run(XMLDocument &doc) {
    cout << "Upgrade from " << m_iFromVersion <<
      " to version " << m_iToVersion << endl;

    XMLElement *pElem = doc.RootElement();
    if (!pElem || run_impl(doc) == UPGRADE_ERROR) {
      return UPGRADE_ERROR;
    }
    
    // finally upgrade the version id
    pElem->SetAttribute("version", m_iToVersion);

    cout << "Upgraded " << m_iCounter << " events/objects" << endl; 
    cout << "Upgrade successfull" << endl;

    return UPGRADE_SUCCESSFULL;
  }
protected:
  virtual EUpgradeResult run_impl(XMLDocument &doc) = 0;
};
#endif
