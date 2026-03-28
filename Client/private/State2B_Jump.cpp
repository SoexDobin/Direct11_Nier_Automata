#include "pch.h"
#include "State2B_Jump.h"

#include <SpdLogger.h>

#include "Pl0000.h"
#include "Pl0000Body.h"
#include "Pl0000StateMachine.h"

State2B_Jump::State2B_Jump(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Jump::Initialize()
{	
	m_EnterAnim.emplace(Pl0000::PL0000_STATE::STAND_TO_JUMP);
	m_EnterAnim.emplace(Pl0000::PL0000_STATE::RUN_TO_JUMP);
	m_EnterAnim.emplace(Pl0000::PL0000_STATE::SPRINT_TO_JUMP);

	return State2B::Initialize();
}

Bool State2B_Jump::StateEnterInvoke()
{
	auto pl0000 = m_Body.lock();
	// 
	switch (auto prevState = m_States.lock()->Get_CurP10000State())
	{
	case Pl0000::PL0000_STATE::IDLE:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::STAND_TO_JUMP), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::RUN:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::RUN_TO_JUMP), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::SPRINT: case Pl0000::PL0000_STATE::DASH:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::SPRINT_TO_JUMP), 0.2f, false);
		return true;
	default: 
		LOG_ERROR(L"[ENTER JUMP] : No Enter state PrevIndex {} ", Helper::To_wString(magic_enum::enum_name(prevState)));
		return false;
	}
}

void State2B_Jump::Update(Float timeDelta)
{
	auto pl0000 = m_Body.lock();
	uint32 curIndex = pl0000->Get_CurrentAnimationIndex();
	uint32 nextIndex = pl0000->Get_NextAnimationIndex();
	Bool isAnimFinished = pl0000->Get_ModelComponent()->Is_AnimationFinished();

	if (m_EnterAnim.contains(curIndex) && isAnimFinished)
	{
		pl0000->Set_Animation(Pl0000::PL0000_STATE::JUMP_ENTER, 0.2f, false);
	}
	else if (curIndex == Pl0000::PL0000_STATE::JUMP_ENTER && isAnimFinished)
	{
		pl0000->Set_Animation(Pl0000::PL0000_STATE::JUMP_HOLD, 0.2f, true);
	}
	else if (curIndex == Pl0000::PL0000_STATE::JUMP_HOLD)
	{
		// 키입력 없으면
		m_States.lock()->Change_State(Pl0000::PL0000_STATE::IDLE);
		// 이전 RUN
		// 이전 State때 DASH or SPRINT
	
		
	}
	
}

void State2B_Jump::Late_Update(Float timeDelta)
{
	
}

void State2B_Jump::StateExitInvoke()
{
	
}

Shared<State2B_Jump> State2B_Jump::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_Jump>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Jump");
		return nullptr;
	}

	return instance;
}


