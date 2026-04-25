#include "pch.h"
#include "Em3000StateMachine.h"
#include "StateEm3000.h"

Em3000StateMachine::Em3000StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: MonsterStateMachine{device, context} {}
Em3000StateMachine::Em3000StateMachine(const Em3000StateMachine& rhs)
	: MonsterStateMachine{rhs} {}

HRESULT Em3000StateMachine::Initialize_Prototype()
{
	return MonsterStateMachine::Initialize_Prototype();
}

HRESULT Em3000StateMachine::Initialize(void* arg)

{
	m_IsSecondPhase = false;
	return MonsterStateMachine::Initialize(arg);
}

void Em3000StateMachine::On_Destroy()
{
	MonsterStateMachine::On_Destroy();
}

void Em3000StateMachine::On_Disable()
{
	MonsterStateMachine::On_Disable();
}

void Em3000StateMachine::On_Enable()
{
	MonsterStateMachine::On_Enable();
}

Bool Em3000StateMachine::Change_State(Em3000::EM3000_STATE state)
{
	wstring tag = Helper::To_wString(magic_enum::enum_name(state));

	return Change_StateByTag(tag);
}


Shared<StateEm3000> Em3000StateMachine::Find_Em3000State(Em3000::EM3000_STATE state)
{
	wstring tag = Helper::To_wString(magic_enum::enum_name(state));

	return static_pointer_cast<StateEm3000>(Find_State(tag));
}

wstring Em3000StateMachine::Get_StateTag(Em3000::EM3000_STATE state)
{
	if (Shared<StateEm3000> searchedState = Find_Em3000State(state))
	{
		return Helper::To_wString(magic_enum::enum_name(state));
	}

	return L"";
}

Em3000::EM3000_STATE Em3000StateMachine::Get_CurEm3000State()
{
	if (nullptr == m_CurrentState)
		return static_cast<Em3000::EM3000_STATE>(0);

	return magic_enum::enum_cast<Em3000::EM3000_STATE>(Helper::To_String(Get_CurrentState()->Get_StateTag())).value();
}

void Em3000StateMachine::Update_State(Float timeDelta)
{
	StateMachine::Update_State(timeDelta);
}

void Em3000StateMachine::Set_SecondPhase()
{
	if (m_IsSecondPhase == true) 
		return;

	m_IsSecondPhase = Change_State(Em3000::EM3000_STATE::GROGGY);
}

Shared<Em3000StateMachine> Em3000StateMachine::Create(const ComPtr<ID3D11Device>& device,
                                                      const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3000StateMachine>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3000StateMachine");
		return nullptr;
	}

	return prototype;
}

Shared<Component> Em3000StateMachine::Clone(void* arg)
{
	auto instance = make_shared<Em3000StateMachine>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3000StateMachine");
		return nullptr;
	}

	return instance;
}
