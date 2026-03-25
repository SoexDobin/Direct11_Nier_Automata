#include "pch.h"
#include "P10000StateMachine.h"

#include "P10000.h"
#include "P10000Input.h"
#include "State2B.h"


P10000StateMachine::P10000StateMachine() : StateMachine{} {}
P10000StateMachine::P10000StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: StateMachine{device, context} {}
P10000StateMachine::P10000StateMachine(const P10000StateMachine& rhs)
	: StateMachine{ rhs } {}

Bool P10000StateMachine::Change_State(P10000::P10000_STATE state)
{
	wstring tag = Helper::To_wString(magic_enum::enum_name(state));
	
	return Change_StateByTag(tag);
}


Shared<State2B> P10000StateMachine::Find_2BState(P10000::P10000_STATE state)
{
	wstring tag = Helper::To_wString(magic_enum::enum_name(state));

	return static_pointer_cast<State2B>(Find_State(tag));
}

wstring P10000StateMachine::Get_StateTag(P10000::P10000_STATE state)
{
	if (Shared<State2B> searchedState = Find_2BState(state))
	{
		return Helper::To_wString(magic_enum::enum_name(state));
	}

	return L"";
}

P10000::P10000_STATE P10000StateMachine::Get_CurP10000State()
{
	return magic_enum::enum_cast<P10000::P10000_STATE>(Helper::To_String(Get_CurrentState()->Get_StateTag())).value();
}
	

HRESULT P10000StateMachine::Initialize_Prototype()
{
	return StateMachine::Initialize_Prototype();
}

HRESULT P10000StateMachine::Initialize(void* arg)
{
	if (FAILED(StateMachine::Initialize(arg)))
		return E_FAIL;

	return S_OK;
}

void P10000StateMachine::On_Destroy()
{
	StateMachine::On_Destroy();
}

void P10000StateMachine::Update_State(Float timeDelta)
{
	m_CurrentState->Update(timeDelta);

	m_CurrentState->Late_Update(timeDelta);
}

Shared<P10000StateMachine> P10000StateMachine::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<P10000StateMachine>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : P10000StateMachine");
		return nullptr;
	}

	return prototype;
}

Shared<Component> P10000StateMachine::Clone(void* arg)
{
	auto instance = make_shared<P10000StateMachine>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : P10000StateMachine");
		return nullptr;
	}

	return instance;
}


