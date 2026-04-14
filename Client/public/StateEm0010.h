#pragma once
#include "State.h"

NS_BEGIN(Client)

class Em0010;
class Em0010Body;
class Em0010Movement;
class MonsterStateMachine;

class CLIENT_DLL StateEm0010 abstract : public State
{
public:
	explicit StateEm0010(const wstring & tag, const Shared<Em0010>&owner);
	virtual ~StateEm0010() override;

protected:
	virtual HRESULT Initialize();

public:
	Float Get_DistanceToTarget() const;
	Vector3 Get_DirectionToTarget() const;
	Bool Has_Target() const;
	Bool Is_TargetFront() const;

protected:
	Weak<Em0010> m_Owner{};
	Weak<Em0010Body> m_Body{};
	Weak<Em0010Movement> m_Movement;
	Weak<MonsterStateMachine> m_States{};

protected:
	const Float n_LeastDistance{ 3.f };
	const Float n_WanderRadius{ 10.f };
};

NS_END
