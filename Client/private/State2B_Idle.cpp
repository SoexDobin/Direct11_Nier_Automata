#include "pch.h"
#include "State2B_Idle.h"
#include "P10000Body.h"
#include <Game.h>
#include <SpdLogger.h>

#include "Model.h"
#include "P10000.h"
#include "P10000StateMachine.h"
#include "WP0070Body.h"
#include "WP0220Body.h"

State2B_Idle::State2B_Idle(const wstring& tag, const Shared<P10000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Idle::Initialize()
{
	return State2B::Initialize();
}

Bool State2B_Idle::StateEnterInvoke()
{
	if (m_Owner.expired())
		return false;
	
	/*
	이전 애니메이션이 Dash 
	이전 애니메이션이 Run
	이전 애니메이션이 Sprint
	 */	

	uint32 prevIndex = m_Body.lock()->Get_CurrentAnimationIndex();
	if (P10000::P10000_STATE::RUN_CYCLE == prevIndex)
	{
		m_Body.lock()->Set_Animation( ETOI(P10000::P10000_STATE::RUN_STOP_L), 0.2f,false);
	}
	else if (P10000::P10000_STATE::SPRINT_CYCLE == prevIndex)
	{
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::SPRINT_STOP_R), 0.2f, false);
	}
	else if (P10000::P10000_STATE::DASH_F == prevIndex)
	{
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::DASH_TO_STAND_F), 0.2f, false);
	}
	else if (P10000::P10000_STATE::DASH_B == prevIndex)
	{
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::DASH_TO_STAND_B), 0.2f, false);
	}
	else if (P10000::P10000_STATE::DASH_R == prevIndex)
	{
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::DASH_TO_STAND_R), 0.2f, false);
	}
	else if (P10000::P10000_STATE::DASH_L == prevIndex)
	{
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::DASH_TO_STAND_L), 0.2f, false);
	}

	if (nullptr == m_States.lock()->Get_CurrentState())
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::IDLE_Neutral), 0.2f, true);

	return true;
}

void State2B_Idle::Update(Float timeDelta)
{
	uint32 curIndex = m_Body.lock()->Get_CurrentAnimationIndex();
	

	if (m_Input.lock()->Is_MousePress(DIMB::LBUTTON))
	{

		return;
	}
	if (m_Input.lock()->Is_MousePress(DIMB::RBUTTON))
	{
		
		return;
	}
	if (m_Input.lock()->Is_WASD_Press())
	{
		if (m_States.lock()->Change_State(P10000::P10000_STATE::RUN))
			return;
	}

	if (curIndex != P10000::P10000_STATE::IDLE_Neutral && m_Body.lock()->Get_ModelComponent()->Is_AnimationFinished() )
	{
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::IDLE_Neutral), 0.2f, true);
	}
}

void State2B_Idle::Late_Update(Float timeDelta)
{

}

void State2B_Idle::StateExitInvoke()
{

}

Shared<State2B_Idle> State2B_Idle::Create(const wstring& tag, const Shared<P10000>& owner)
{
	auto instance = make_shared<State2B_Idle>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Idle");
		return nullptr;
	}

	return instance;
}
