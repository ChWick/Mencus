#ifndef _GAME_INPUT_COMMAND_
#define _GAME_INPUT_COMMAND_

enum EGameInputCommandTypes {
  GIC_RIGHT,
  GIC_LEFT,
  GIC_JUMP,
  GIC_ENTER_LINK,
  GIC_ATTACK,
  GIC_ACTIVATE,
};

class CGameInputCommand {
public:
private:
  const EGameInputCommandTypes m_eType;	//!< Type of the input command
  const float m_fValue;		        //!< Input strength between 0 and 1

public:
  CGameInputCommand(const EGameInputCommandTypes eType, float fValue = 1)
    : m_eType(eType), m_fValue(fValue) {
  }

  EGameInputCommandTypes getType() const {return m_eType;}
  float getValue() const {return m_fValue;}
};

#endif
