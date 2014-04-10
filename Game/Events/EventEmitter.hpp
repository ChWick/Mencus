#ifndef _EVENT_EMITTER_HPP_
#define _EVENT_EMITTER_HPP_


#include <tinyxml2.h>
#include <string>
#include "XMLHelper.hpp"

class CEntity;
class CMap;

namespace EventEmitter {
  enum ERepeatType {
    REPEAT_NONE,
    REPEAT_INFINITE
  };
  enum EEmitter {
    EMIT_ON_CREATE,
    EMIT_ON_DESTROY,
    EMIT_ON_USER,
    EMIT_ON_COLLISION,
    EMIT_ON_MESSAGE_BOX_PAGE_CHANGE,
  };
  std::string toString(EEmitter eEmitter);
  EEmitter parseEmitter(const std::string &sString);

  std::string toString(ERepeatType eRepeatType);
  ERepeatType parseRepeatType(const std::string &sString);

  class CEmitter {
  private:
    const EEmitter m_eType;
    const ERepeatType m_eRepeatType;
  public:
    CEmitter(EEmitter eType, const tinyxml2::XMLElement *pElem, ERepeatType eRepeatType = REPEAT_NONE) 
      : m_eType(eType),
	m_eRepeatType(parseRepeatType(XMLHelper::Attribute(pElem, "repeat", toString(eRepeatType)))) {
    }
    virtual ~CEmitter() {
    }

    virtual void init(const CMap &map) {}

    EEmitter getType() const {return m_eType;}
    ERepeatType getRepeatType() const {return m_eRepeatType;}

    virtual void writeToXMLElement(tinyxml2::XMLElement *pElem) const {
      pElem->SetAttribute("emitter", toString(m_eType).c_str());
      pElem->SetAttribute("repeat", toString(m_eRepeatType).c_str());
    }
  };

  class COnCreate : public CEmitter  {
  public:
    COnCreate(const tinyxml2::XMLElement *pElem)
      : CEmitter(EMIT_ON_CREATE, pElem) {
    }
  };

  class COnDestroy : public CEmitter  {
  public:
    COnDestroy(const tinyxml2::XMLElement *pElem)
      : CEmitter(EMIT_ON_DESTROY, pElem) {
    }
  };

  class COnUser : public CEmitter  {
  public:
    COnUser(const tinyxml2::XMLElement *pElem)
      : CEmitter(EMIT_ON_USER, pElem) {
    }
  };

  class COnCollision : public CEmitter {
  private:
    const CEntity *m_pEntity;
    const std::string m_sSrcID;
  public:
    COnCollision(const tinyxml2::XMLElement *pElem)
      : CEmitter(EMIT_ON_COLLISION, pElem),
	m_sSrcID(XMLHelper::Attribute(pElem, "src_id")) {
    }

    virtual void init(const CMap &map);

    virtual void writeToXMLElement(tinyxml2::XMLElement *pElem) const {
      CEmitter::writeToXMLElement(pElem);

      XMLHelper::SetAttribute(pElem, "src_id", m_sSrcID);
    }

    const std::string &getSrcID() const {return m_sSrcID;}
    const CEntity *getEntity() const {return m_pEntity;}
  };

  class COnMessageBoxPageChange : public CEmitter {
  private:
    const std::string m_sSrcID;
    const int m_iPageID;
  public:
    COnMessageBoxPageChange(const tinyxml2::XMLElement *pElem)
      : CEmitter(EMIT_ON_MESSAGE_BOX_PAGE_CHANGE, pElem),
	m_sSrcID(XMLHelper::Attribute(pElem, "src_id")),
	m_iPageID(XMLHelper::IntAttribute(pElem, "src_page")) {
    }

    virtual void writeToXMLElement(tinyxml2::XMLElement *pElem) const {
      CEmitter::writeToXMLElement(pElem);

      XMLHelper::SetAttribute(pElem, "src_id", m_sSrcID);
      XMLHelper::SetAttribute(pElem, "src_page", m_iPageID);
    }

    const std::string &getSrcID() const {return m_sSrcID;}
    const int getPageID() const {return m_iPageID;}
  };


  class CCreator {
  public:
    static CEmitter *create(const tinyxml2::XMLElement *pElem);
  };

};

#endif
