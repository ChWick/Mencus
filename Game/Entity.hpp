#ifndef _ENTITY_HPP_
#define _ENTITY_HPP_

#include <string>
#include <list>
#include <OgreVector2.h>
#include "BoundingBox2d.hpp"
#include <tinyxml2.h>
#include "OutputStyle.hpp"

class CEvent;
class CMap;

//! Class for an entity in the map
class CEntity {
protected:
  std::string m_sID;

  CMap &m_Map;

  CEntity *m_pParent;
  std::list<CEntity*> m_lChildren;
  std::list<CEvent*> m_lEvents;

  Ogre::Vector2 m_vPosition;	                        //!< Position of the entity 
  Ogre::Vector2 m_vSize;				//!< Size of the entity
  Ogre::Vector2 m_vScale;				//!< Scale of the entity
  CBoundingBox2d m_bbRelativeBoundingBox;		//!< Bounding box, realative position
public:
  CEntity(CMap &map, const std::string &sID, CEntity *pParent);
  CEntity(CMap &map,
	  CEntity *pParent,
	  const tinyxml2::XMLElement *pElem,
	  const Ogre::Vector2 &vDefaultPosition = Ogre::Vector2::ZERO,
	  const Ogre::Vector2 &vDefaultSize = Ogre::Vector2::ZERO,
	  const Ogre::Vector2 &vDefaultScale = Ogre::Vector2::UNIT_SCALE);
  CEntity(const CEntity &src);
  virtual ~CEntity();

  // tree access
  void attachTo(CEntity *pParent);
  CEntity *getParent() {return m_pParent;}
  const CEntity *getParent() const {return m_pParent;}
  std::list<CEntity *> &getChildren() {return m_lChildren;}
  const std::list<CEntity *> &getChildren() const {return m_lChildren;}
  
  // events access
  std::list<CEvent*> &getEvents() {return m_lEvents;}
  const std::list<CEvent*> &getEvents() const {return m_lEvents;}
  void addEvent(CEvent *pEvent) {m_lEvents.push_back(pEvent);}
  void destroyEvent(CEvent *pEvent);

  // methods
  virtual void init();
  virtual void exit();
  
  virtual void update(Ogre::Real tpf);
  virtual void render(Ogre::Real tpf);

  virtual void writeToXMLElement(tinyxml2::XMLElement *pElement, EOutputStyle eStyle) const;


  // getter setter
  // =============

  // general
  const std::string &getID() const {return m_sID;}
  std::string &getID() {return m_sID;}
  void setID(const std::string &sID) {m_sID = sID;}

  Ogre::Vector2 &getPosition() {return m_vPosition;}
  const Ogre::Vector2 &getPosition() const {return m_vPosition;}
  void setPosition(const Ogre::Vector2 &vPos) {m_vPosition = vPos;}
  void translate(const Ogre::Vector2 &vOffset) {m_vPosition += vOffset;}

  Ogre::Vector2 getCenter() const {return m_vPosition + m_vSize / 2;}
  void setCenter(const Ogre::Vector2 &vCenter) {m_vPosition = vCenter - m_vSize / 2;}

  Ogre::Vector2 &getSize() {return m_vSize;}
  const Ogre::Vector2 &getSize() const {return m_vSize;}
  void setSize(const Ogre::Vector2 &vSize) {m_vSize = vSize;}

  Ogre::Vector2 &getScale() {return m_vScale;}
  const Ogre::Vector2 &getScale() const {return m_vScale;}
  void setScale(const Ogre::Vector2 &vScale) {m_vScale = vScale;}

  const CBoundingBox2d &getBoundingBox() const {return m_bbRelativeBoundingBox;}
  CBoundingBox2d getWorldBoundingBox() const {return m_bbRelativeBoundingBox.translate(m_vPosition);}
  void setRelativeBoundingBox(const CBoundingBox2d &bb) {m_bbRelativeBoundingBox = bb;}

  // for events
  
};


#endif
