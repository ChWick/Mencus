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

#ifndef _EVENT_EMITTER_HPP_
#define _EVENT_EMITTER_HPP_


#include <tinyxml2.h>
#include <string>
#include "XMLHelper.hpp"
#include "Message.hpp"

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
    EMIT_ON_MESSAGE,
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
	m_eRepeatType((pElem) ? parseRepeatType(XMLHelper::Attribute(pElem, "repeat", toString(eRepeatType))) : REPEAT_INFINITE) {
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
	m_pEntity(NULL),
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

  class COnMessage : public CEmitter {
  private:
    const CMessage::EMessageTypes m_eMessageType;
    const bool m_bBoolValue;
  public:
    COnMessage(const tinyxml2::XMLElement *pElem)
      : CEmitter(EMIT_ON_MESSAGE, pElem),
	m_eMessageType(CMessage::parseMessageType(XMLHelper::Attribute(pElem, "emit_msg_type"))),
	m_bBoolValue(XMLHelper::BoolAttribute(pElem, "emit_msg_bool")) {
     }
      
    virtual void writeToXMLElement(tinyxml2::XMLElement *pElem) const {
      CEmitter::writeToXMLElement(pElem);
      
      XMLHelper::SetAttribute(pElem, "emit_msg_type", CMessage::toString(m_eMessageType));
      XMLHelper::SetAttribute(pElem, "emit_msg_bool", m_bBoolValue);
    }
    
    const CMessage::EMessageTypes getMessageType() const {return m_eMessageType;}
    const bool getBoolValue() const {return m_bBoolValue;}
  };


  class CCreator {
  public:
    static CEmitter *create(const tinyxml2::XMLElement *pElem);
  };

};

#endif
