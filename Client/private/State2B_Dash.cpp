#include "pch.h"
#include "State2B_Dash.h"

#include "Pl0000.h"
#include "Pl0000Body.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "Pl0000Input.h"
#include "Pl0000StateMachine.h"
#include "Model.h"

State2B_Dash::State2B_Dash(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Dash::Initialize()
{
	return S_OK;
}

Bool State2B_Dash::StateEnterInvoke()
{
	if (m_Input.lock()->Is_WA_DoubleClick())
	{

	}

	if (m_Input.lock()->Is_KeyMultiClick(DIKEYBOARD_W))
	{
		m_Body.lock()->Set_Animation(ETOI(Pl0000::P10000_STATE::DASH_F), 0.2f, true);
	}
	if (m_Input.lock()->Is_KeyMultiClick(DIKEYBOARD_S))
	{
		m_Body.lock()->Set_Animation(ETOI(Pl0000::P10000_STATE::DASH_B), 0.2f, true);
	}
	if (m_Input.lock()->Is_KeyMultiClick(DIKEYBOARD_D))
	{
		m_Body.lock()->Set_Animation(ETOI(Pl0000::P10000_STATE::DASH_R), 0.2f, true);
	}
	if (m_Input.lock()->Is_KeyMultiClick(DIKEYBOARD_A))
	{
		m_Body.lock()->Set_Animation(ETOI(Pl0000::P10000_STATE::DASH_L), 0.2f, true);
	}

	return true;
}

void State2B_Dash::Update(Float timeDelta)
{
	if (m_Owner.expired())
		return;

	auto owner = m_Owner.lock();
	auto body = static_pointer_cast<Pl0000Body>(owner->Find_PartObject(L"Pl0000Body"));

	if (body && body->Get_ModelComponent()->Is_AnimationFinished())
	{
		auto input = owner->Get_Component<Pl0000Input>();
		auto stateMachine = owner->Get_Component<Pl0000StateMachine>();

		Bool isMoveInput = input->Is_KeyPress(DIKEYBOARD_W) ||
						   input->Is_KeyPress(DIKEYBOARD_A) ||
						   input->Is_KeyPress(DIKEYBOARD_S) ||
						   input->Is_KeyPress(DIKEYBOARD_D);
		
		if (isMoveInput)
		{
			stateMachine->Change_State(Pl0000::SPRINT);
		}
		else
		{
			stateMachine->Change_State(Pl0000::IDLE_Neutral);
		}
	}
}

void State2B_Dash::Late_Update(Float timeDelta)
{
}

void State2B_Dash::StateExitInvoke()
{
}

Shared<State2B_Dash> State2B_Dash::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_Dash>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : State2B_Dash");
		return nullptr;
	}

	return instance;
}
