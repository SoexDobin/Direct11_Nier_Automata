#include "pch.h"
#include "State2B_Idle.h"
#include "Pl0000Body.h"
#include <Game.h>
#include <SpdLogger.h>

#include "Model.h"
#include "Pl0000.h"
#include "Pl0000StateMachine.h"
#include "WP0070Body.h"
#include "WP0220Body.h"

State2B_Idle::State2B_Idle(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Idle::Initialize()
{
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::RUN_STOP_L));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::SPRINT_STOP_R));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::JUMP_TO_STAND));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_F));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_B));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_R));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_L));

	return State2B::Initialize();
}

Bool State2B_Idle::StateEnterInvoke()
{
	auto pl0000 = m_Body.lock();

	switch (const uint32 prevIndex = pl0000->Get_CurrentAnimationIndex())
	{
	case Pl0000::PL0000_STATE::RUN_CYCLE:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::RUN_STOP_L), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::SPRINT_CYCLE:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::SPRINT_STOP_R), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::JUMP_HOLD:
		pl0000->Set_Animation(Pl0000::PL0000_STATE::JUMP_TO_STAND, 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::DASH_F:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_F), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::DASH_B:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_B), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::DASH_R:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_R), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::DASH_L:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_L), 0.2f, false);
		return true;
	default:
		{
			if (m_InitializeState)
			{
				m_Body.lock()->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_TO_STAND_L), 0.2f, false);

				m_LightWeapon.lock()->Get_Transform()->Set_WorldMatrix(m_Owner.lock()->Get_SheathingMatrix());
				m_HeavyWeapon.lock()->Get_Transform()->Set_WorldMatrix(m_Owner.lock()->Get_SheathingMatrix());

				return true;
			}
			LOG_ERROR(L"[ENTER IDLE] : No Enter state PrevIndex {} ", prevIndex);
			return false;
		}
	}
}

void State2B_Idle::Update(Float timeDelta)
{
	uint32 curIndex = m_Body.lock()->Get_CurrentAnimationIndex();
	uint32 nextIndex = m_Body.lock()->Get_NextAnimationIndex();
	

	if (m_Input.lock()->Is_MousePress(DIMB::LBUTTON))
	{

		return;
	}
	if (m_Input.lock()->Is_MousePress(DIMB::RBUTTON))
	{
		
		return;
	}
	if (m_Input.lock()->Is_KeyDown(static_cast<uByte>(DIKEYBOARD_SPACE)))
	{
		if (m_States.lock()->Change_State(Pl0000::PL0000_STATE::JUMP))
			return;
	}
	if (m_Input.lock()->Is_WASD_Press())
	{
		if (m_States.lock()->Change_State(Pl0000::PL0000_STATE::RUN))
			return;
	}
	

	if (m_EnterAnim.contains(curIndex) && m_Body.lock()->Get_ModelComponent()->Is_AnimationFinished())
	{
		m_Body.lock()->Set_Animation(ETOI(Pl0000::PL0000_STATE::IDLE_STAND_TO_Neutral), 0.5f, false);
	}
	else if (curIndex == Pl0000::PL0000_STATE::IDLE_STAND_TO_Neutral && m_Body.lock()->Get_ModelComponent()->Is_AnimationFinished())
	{
		m_Body.lock()->Set_Animation(ETOI(Pl0000::PL0000_STATE::IDLE_Neutral), 0.5f, true);
	}
}

void State2B_Idle::Late_Update(Float timeDelta)
{

}

void State2B_Idle::StateExitInvoke()
{

}

Shared<State2B_Idle> State2B_Idle::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_Idle>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Idle");
		return nullptr;
	}

	return instance;
}
