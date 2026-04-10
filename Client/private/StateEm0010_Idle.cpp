#include "pch.h"
#include "StateEm0010_Idle.h"

#include <SpdLogger.h>

#include "Em0010.h"
#include "Em0010Body.h"
#include "MonsterStateMachine.h"

StateEm0010_Idle::StateEm0010_Idle(const wstring& tag, const Shared<Em0010>& owner)
	: StateEm0010(tag, owner) {}

HRESULT StateEm0010_Idle::Initialize()
{
	using msState = MonsterStateMachine::MONSTER_STATE;
	m_EnterAnim.emplace(ETOI(msState::MOVE));

	return StateEm0010::Initialize();
}


Bool StateEm0010_Idle::StateEnterInvoke()
{
	if (m_Owner.lock()->Is_Dead()) return false;
	auto em0010 = m_Body.lock();

	switch (auto prevState = m_States.lock()->Get_CurMonsterState())
	{
	case MonsterStateMachine::MONSTER_STATE::Hit:
		em0010->Set_Animation(ETOI(Em0010::EM0010_STATE::IDLE), 0.2f, true);
		return true;
	case MonsterStateMachine::MONSTER_STATE::MOVE:
		em0010->Set_Animation(ETOI(Em0010::EM0010_STATE::WALK_END), 0.2f, true);
		return true;
	case MonsterStateMachine::MONSTER_STATE::CHASE:
		em0010->Set_Animation(ETOI(Em0010::EM0010_STATE::SPRINT_END), 0.2f, true);
		return true;
	default:
	{
		if (m_InitializeState)
		{
			em0010->Set_Animation(ETOI(Em0010::EM0010_STATE::IDLE), 0.2f, true);
			m_InitializeState = false;

			return true;
		}
		LOG_ERROR(L"[ENTER IDLE] : No Enter state PrevIndex {} ", ETOI(prevState));
		return false;
	}
	}
}

void StateEm0010_Idle::Update(Float timeDelta)
{
	if (Has_Target())
	{
		m_States.lock()->Change_State(MonsterStateMachine::MONSTER_STATE::CHASE);
	}
}

void StateEm0010_Idle::Late_Update(Float timeDelta)
{
	
}

void StateEm0010_Idle::StateExitInvoke()
{
	
}

Shared<StateEm0010_Idle> StateEm0010_Idle::Create(const wstring& tag, const Shared<Em0010>& owner)
{
	auto instance = make_shared<StateEm0010_Idle>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm0010_Idle");
		return nullptr;
	}

	return instance;
}
