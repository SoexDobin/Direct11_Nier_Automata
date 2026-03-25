#include "pch.h"
#include "State2B_Dash.h"

#include "P10000.h"
#include "P10000Body.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "P10000Input.h"
#include "P10000StateMachine.h"
#include "Model.h"

State2B_Dash::State2B_Dash(const wstring& tag, const Shared<P10000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Dash::Initialize()
{
	return S_OK;
}

Bool State2B_Dash::StateEnterInvoke()
{
	if (m_Owner.expired())
		return false;

	auto owner = m_Owner.lock();
	auto enumVal = magic_enum::enum_cast<P10000::P10000_STATE>(Helper::To_String(m_StateTag)).value();

	auto body = static_pointer_cast<P10000Body>(owner->Find_PartObject(L"P10000Body"));
	if (body) {
		body->Set_Animation(static_cast<uint32>(enumVal), 0.1f, false);
	}

	//auto sword = static_pointer_cast<WP0070Body>(owner->Find_PartObject(L"WP0070Body"));
	//if (sword) sword->Set_Animation(static_cast<uint32>(enumVal), 0.1f, false);
	//
	//auto greaterSword = static_pointer_cast<WP0220Body>(owner->Find_PartObject(L"WP0220Body"));
	//if (greaterSword) greaterSword->Set_Animation(static_cast<uint32>(enumVal), 0.1f, false);

	return true;
}

void State2B_Dash::Update(Float timeDelta)
{
	if (m_Owner.expired())
		return;

	auto owner = m_Owner.lock();
	auto body = static_pointer_cast<P10000Body>(owner->Find_PartObject(L"P10000Body"));

	if (body && body->Get_ModelComponent()->Is_AnimationFinished())
	{
		auto input = owner->Get_Component<P10000Input>();
		auto stateMachine = owner->Get_Component<P10000StateMachine>();

		Bool isMoveInput = input->Is_KeyPress(DIKEYBOARD_W) ||
						   input->Is_KeyPress(DIKEYBOARD_A) ||
						   input->Is_KeyPress(DIKEYBOARD_S) ||
						   input->Is_KeyPress(DIKEYBOARD_D);
		
		if (isMoveInput)
		{
			stateMachine->Change_State(P10000::SPRINT);
		}
		else
		{
			stateMachine->Change_State(P10000::IDLE_Neutral);
		}
	}
}

void State2B_Dash::Late_Update(Float timeDelta)
{
}

void State2B_Dash::StateExitInvoke()
{
}

Shared<State2B_Dash> State2B_Dash::Create(const wstring& tag, const Shared<P10000>& owner)
{
	auto instance = make_shared<State2B_Dash>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : State2B_Dash");
		return nullptr;
	}

	return instance;
}
