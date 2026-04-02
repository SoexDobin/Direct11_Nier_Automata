#pragma once
#include "State.h"

NS_BEGIN(Engine)
class Camera;
NS_END

NS_BEGIN(Client)
	class Pl0000;
	class Pl0000Input;
	class Pl0000Movement;
	class Pl0000StateMachine;
	class Pl0000Body;
	class WP0070Body;
	class WP0220Body;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL State2B abstract : public State
{
public:
	explicit State2B(const wstring& tag, const Shared<Pl0000>& owner);
	virtual ~State2B() override;

protected:
	virtual HRESULT Initialize();

public:
	Vector3 Calculate_Direction();

protected:
	Weak<Pl0000> m_Owner{};
	Weak<Camera> m_MainCamera{};

	Weak<Pl0000Body> m_Body{};
	Weak<WP0070Body> m_LightWeapon{};
	Weak<WP0220Body> m_HeavyWeapon{};

	Weak<Pl0000Input> m_Input{};
	Weak<Pl0000Movement> m_Movement;
	Weak<Pl0000StateMachine> m_States{};
};

NS_END
