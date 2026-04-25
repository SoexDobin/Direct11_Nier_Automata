#include "pch.h"
#include "StateEm3000_Idle.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"
#include "Random_Helper.h"

StateEm3000_Idle::StateEm3000_Idle(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{tag, owner} {}

HRESULT StateEm3000_Idle::Initialize()
{
	return StateEm3000::Initialize();
}

Bool StateEm3000_Idle::StateEnterInvoke()
{
	auto em3000 = m_Owner.lock();
	auto em3000Body = m_Body.lock();
	
	em3000Body->Set_Animation(ETOI(Em3000::EM3000_STATE::IDLE), 0.1f, true);

	m_delayAcc = Helper::Random_Float(0.5f, 2.0f);


	return true;
}

void StateEm3000_Idle::Update(Float timeDelta)
{
	if (m_delayAcc > 0.f) return;

	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();

	if (Has_MeleeTarget())
	{
		state->Change_State(Em3000::EM3000_STATE::PHASE1_MELEE);
		return;
	}

	if (auto goMelee = Helper::Random_Bool(0.3))
	{
		// TODO 근접 검사
		if (Has_MeleeTarget())
		{
			state->Change_State(Em3000::EM3000_STATE::PHASE1_MELEE);
		}
	}
	else
	{
		state->Change_State(Em3000::EM3000_STATE::PHASE1_RANGE);
	}


}

void StateEm3000_Idle::Late_Update(Float timeDelta)
{
	m_delayAcc -= timeDelta;
}

void StateEm3000_Idle::StateExitInvoke()
{
	m_delayAcc = 0.f;
}

Shared<StateEm3000_Idle> StateEm3000_Idle::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Idle>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Idle");
		return nullptr;
	}

	return prototype;
}