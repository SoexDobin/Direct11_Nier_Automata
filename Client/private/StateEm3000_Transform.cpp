#include "pch.h"
#include "StateEm3000_Transform.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"

StateEm3000_Transform::StateEm3000_Transform(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{tag, owner}
{
}

HRESULT StateEm3000_Transform::Initialize()
{
	return StateEm3000::Initialize();
}

Bool StateEm3000_Transform::StateEnterInvoke()
{
	m_Body.lock()->Set_Animation(ETOI(Em3000::EM3000_STATE::PHASE_CHANGE1), 0.1f, false);

	return true;
}

void StateEm3000_Transform::Update(Float timeDelta)
{
	if (Bool isFinished = m_Body.lock()->Is_AnimationFinished())
	{
		// TODO : 2페이즈
		//m_States.lock()->Change_State();
	}
}

void StateEm3000_Transform::Late_Update(Float timeDelta)
{
	
}

void StateEm3000_Transform::StateExitInvoke()
{
	
}

Shared<StateEm3000_Transform> StateEm3000_Transform::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Transform>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Transform");
		return nullptr;
	}

	return prototype;
}

