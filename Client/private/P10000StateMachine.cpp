#include "pch.h"
#include "P10000StateMachine.h"

#include "P10000.h"


P10000StateMachine::P10000StateMachine()
{
}

P10000StateMachine::P10000StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
}

P10000StateMachine::P10000StateMachine(const P10000StateMachine& rhs)
{
}

Bool P10000StateMachine::ChangeState(P10000::P10000_STATE state)
{
	
	return 
}


Shared<State2B> P10000StateMachine::Find_2BState(P10000::P10000_STATE state)
{
	
}

wstring P10000StateMachine::Get_StateTag(P10000::P10000_STATE state)
{
	
}

HRESULT P10000StateMachine::Initialize_Prototype()
{
	return StateMachine::Initialize_Prototype();
}

HRESULT P10000StateMachine::Initialize(void* arg)
{
	return StateMachine::Initialize(arg);
}

void P10000StateMachine::On_Destroy()
{
	StateMachine::On_Destroy();
}

Shared<P10000StateMachine> P10000StateMachine::Create(const ComPtr<ID3D11Device>& device,
                                                      const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<P10000StateMachine>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : P10000StateMachine");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> P10000StateMachine::Clone(void* arg)
{
	auto instance = make_shared<P10000StateMachine>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : P10000StateMachine");
		return nullptr;
	}

	return instance;
}


