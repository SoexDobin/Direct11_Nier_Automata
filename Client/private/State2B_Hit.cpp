#include "pch.h"
#include "State2B_Hit.h"

#include <SpdLogger.h>

#include "Pl0000.h"
#include "Pl0000StateMachine.h"
#include <Random_Helper.h>

State2B_Hit::State2B_Hit(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner} {}

using plState = Pl0000::PL0000_STATE;
HRESULT State2B_Hit::Initialize()
{
	m_HitAnim.emplace(ETOI(Pl0000::PL0000_STATE::HIT1));
	m_HitAnim.emplace(ETOI(Pl0000::PL0000_STATE::HIT2));
	m_HitAnim.emplace(ETOI(Pl0000::PL0000_STATE::HIT3));
	m_HitAnim.emplace(ETOI(Pl0000::PL0000_STATE::HIT4));

	return State2B::Initialize();
}

Bool State2B_Hit::StateEnterInvoke()
{
	auto pl0000Body = m_Body.lock();
	uint32 animIndex = Helper::Random_Int(ETOI(Pl0000::PL0000_STATE::HIT1), ETOI(Pl0000::PL0000_STATE::HIT4));
	pl0000Body->Set_Animation(animIndex, 0.1f, false);

	return true;
}

void State2B_Hit::Update(Float timeDelta)
{
	auto pl0000Body = m_Body.lock();
	auto state = m_States.lock();

	uint32 animIndex = pl0000Body->Get_CurrentAnimationIndex();
	Bool isFinished = pl0000Body->Is_AnimationFinished();
	Float progress = pl0000Body->Get_AnimationProgress();

	if (m_HitAnim.contains(animIndex) && progress >= 0.25f)
	{
		if (m_Input.lock()->Is_MouseDown(DIMB::LBUTTON) || m_Input.lock()->Is_MouseDown(DIMB::RBUTTON))
		{
			if (m_States.lock()->Change_State(Pl0000::PL0000_STATE::ATTACK_GROUND))
				return;
		}

		if (m_Input.lock()->Is_KeyDown(UBYTE(DIKEYBOARD_SPACE)))
		{
			if (m_States.lock()->Change_State(Pl0000::PL0000_STATE::JUMP))
				return;
		}
		if (m_Input.lock()->Is_WASD_DoubleClick())
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::EVADE);
			return;
		}
		if (m_Input.lock()->Is_WASD_Press())
		{
			if (m_States.lock()->Change_State(Pl0000::PL0000_STATE::RUN))
				return;
		}
	}
	else if (isFinished)
	{
		state->Change_State(plState::IDLE);
	}
}

void State2B_Hit::Late_Update(Float timeDelta)
{
	
}

void State2B_Hit::StateExitInvoke()
{
	
}


Shared<State2B_Hit> State2B_Hit::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_Hit>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Hit");
		return nullptr;
	}

	return instance;
}

