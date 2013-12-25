#ifndef HITABLE_OBJECT_HPP
#define HITABLE_OBJECT_HPP

class CHitableObject {
private:
  const Ogre::Real m_fMaximumHitpoints;
  Ogre::Real m_fHitpoints;
public:
  CHitableObject(Ogre::Real fMaxHitpoints)
    : m_fMaximumHitpoints(fMaxHitpoints),
      m_fHitpoints(fMaxHitpoints) {
  }

  void takeDamage(Ogre::Real fHitpoints) {
    m_fHitpoints -= fHitpoints;
    damageTakenCallback();
    if (m_fHitpoints <= 0) {
      killedByDamageCallback();
    }
  }

  Ogre::Real getHitpoints() const {return m_fHitpoints;}
  Ogre::Real getMaximumHitpoints() const {return m_fMaximumHitpoints;}

protected:
  virtual void damageTakenCallback() {}
  virtual void killedByDamageCallback() {}
};

#endif
