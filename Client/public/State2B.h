#pragma once
#include "State.h"

NS_BEGIN(Client)
	class P10000;
	class P10000Input;
	class P10000StateMachine;
	class P10000Body;
	class WP0070Body;
	class WP0220Body;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL State2B abstract : public State
{
public:
	explicit State2B(const wstring& tag, const Shared<P10000>& owner);
	virtual ~State2B() override;

protected:
	virtual HRESULT Initialize();

protected:
	Weak<P10000> m_Owner{};

	Weak<P10000Body> m_Body{};
	Weak<WP0070Body> m_LightWeapon{};
	Weak<WP0220Body> m_HeavyWeapon{};
	Weak<P10000Input> m_Input{};
	Weak<P10000StateMachine> m_States{};
};

NS_END
