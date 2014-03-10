#ifndef _GAME_INPUT_COMMAND_
#define _GAME_INPUT_COMMAND_

enum EGameInputCommandTypes {
  GIC_RIGHT,
  GIC_LEFT,
  GIC_JUMP,
  GIC_ENTER_LINK,
  GIC_ATTACK,
  GIC_ACTIVATE,
  GIC_USE_HEALTH_POTION,
  GIC_USE_MANA_POTION,
  GIC_CHANGE_WEAPON,
};

class CGameInputCommand {
public:
private:
  const EGameInputCommandTypes m_eType;	//!< Type of the input command
  const float m_fValue;		        //!< Input strength between 0 and 1
  const int m_iValue;                   //!< Integer value, useful in different cases

public:
  CGameInputCommand(const EGameInputCommandTypes eType, float fValue = 1)
    : m_eType(eType), m_fValue(fValue), m_iValue(0) {
  }
  CGameInputCommand(const EGameInputCommandTypes eType, unsigned int uiValue)
    : m_eType(eType), m_fValue(0), m_iValue(uiValue) {
  }
  CGameInputCommand(const EGameInputCommandTypes eType, int iValue)
    : m_eType(eType), m_fValue(0), m_iValue(iValue) {
  }

  EGameInputCommandTypes getType() const {return m_eType;}
  float getValue() const {return m_fValue;}
  float getFloatValue() const {return m_fValue;}
  int getIntValue() const {return m_iValue;}
  
};

#endif
