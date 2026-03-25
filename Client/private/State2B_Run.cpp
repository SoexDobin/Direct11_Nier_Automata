#include "pch.h"
#include "State2B_Run.h"

#include <SpdLogger.h>
#include "Model.h"
#include "StateMachine.h"
#include "P10000.h"
#include "P10000Body.h"
#include "P10000StateMachine.h"
#include "WP0070Body.h"
#include "WP0220Body.h"

State2B_Run::State2B_Run(const wstring& tag, const Shared<P10000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Run::Initialize()
{
	return State2B::Initialize();
}

Bool State2B_Run::StateEnterInvoke()
{

	uint32 prevIndex = m_Body.lock()->Get_CurrentAnimationIndex();
	if (P10000::P10000_STATE::IDLE_Neutral == prevIndex)
	{
		m_Body.lock()->Set_Animation(ETOI(P10000::P10000_STATE::RUN_CYCLE), 0.2f, true);
	}

	return true;
}

void State2B_Run::Update(Float timeDelta)
{
	if (m_Input.lock()->Is_WASD_DoubleClick())
	{
		//m_Body->Set_Animation(ETOI(P10000::P10000_STATE::DASH_F), 0.2f, true);
		//m_Body->Set_Animation(ETOI(P10000::P10000_STATE::DASH_B), 0.2f, true);
		//m_Body->Set_Animation(ETOI(P10000::P10000_STATE::DASH_R), 0.2f, true);
		//m_Body->Set_Animation(ETOI(P10000::P10000_STATE::DASH_L), 0.2f, true);
		m_States.lock()->Change_State(P10000::P10000_STATE::DASH);
	}

	if (m_Input.lock()->Is_WASD_Press()) return;

	m_States.lock()->Change_State(P10000::P10000_STATE::IDLE);
}

void State2B_Run::Late_Update(Float timeDelta)
{
	
}

void State2B_Run::StateExitInvoke()
{
	
}

Shared<State2B_Run> State2B_Run::Create(const wstring& tag, const Shared<P10000>& owner)
{
	auto instance = make_shared<State2B_Run>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Run");
		return nullptr;
	}

	return instance;
}
