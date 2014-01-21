#ifndef HITABLE_OBJECT_HPP
#define HITABLE_OBJECT_HPP

#include <algorithm>

class CHitableObject {
private:
  const Ogre::Real m_fMaximumHitpoints;
  Ogre::Real m_fHitpoints;
  bool m_bInvunerable;
public:
  CHitableObject(Ogre::Real fMaxHitpoints)
    : m_fMaximumHitpoints(fMaxHitpoints),
      m_fHitpoints(fMaxHitpoints),
      m_bInvunerable(false) {
  }

  void takeDamage(Ogre::Real fHitpoints) {
    if (m_bInvunerable) {return;}

    m_fHitpoints -= fHitpoints;
    damageTakenCallback();
    if (m_fHitpoints <= 0) {
      killedByDamageCallback();
    }
  }

  Ogre::Real getHitpoints() const {return m_fHitpoints;}
  Ogre::Real getMaximumHitpoints() const {return m_fMaximumHitpoints;}
  bool isInvunerable() const {return m_bInvunerable;}
  void setInvunerable(bool bInvunerable) {m_bInvunerable = bInvunerable;}
  void addHitpoints(Ogre::Real fHitpoints) {m_fHitpoints = std::min<Ogre::Real>(m_fHitpoints + fHitpoints, m_fMaximumHitpoints);}
protected:
  virtual void damageTakenCallback() {}
  virtual void killedByDamageCallback() {}
};

#endif
