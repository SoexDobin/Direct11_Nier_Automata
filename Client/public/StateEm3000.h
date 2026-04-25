#pragma once
#include "MonsterSight.h"
#include "State.h"

NS_BEGIN(Client)
	

class Em3000;
class Em3000Body;
class Em3000Movement;
class Em3000StateMachine;
class Em3000MeleeSight;

class CLIENT_DLL StateEm3000 abstract : public State
{
public:
	explicit StateEm3000(const wstring& tag, const Shared<Em3000>& owner);
	virtual ~StateEm3000() override;

protected:
	virtual HRESULT Initialize();

public:
	Bool Has_MeleeTarget() const;
	Bool Has_FrontMeleeTarget() const;

protected:
	Weak<Em3000> m_Owner{};
	Weak<Em3000Body> m_Body{};
	Weak<Em3000Movement> m_Movement;
	Weak<Em3000StateMachine> m_States{};
	Weak<Em3000MeleeSight> m_MeleeSight{};
	
};

NS_END