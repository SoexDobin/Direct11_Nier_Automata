#include "StateMachine.h"

NS_BEGIN(Engine)

StateMachine::StateMachine()
	: ScriptComponent()
{
}

StateMachine::StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ScriptComponent(device, context)
{
}

StateMachine::StateMachine(const StateMachine& rhs)
	: ScriptComponent(rhs)
{
}

HRESULT StateMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT StateMachine::Initialize(void* arg)
{
	return S_OK;
}

void StateMachine::Update(Float timeDelta)
{
	if (nullptr != m_pCurrentState)
	{
		m_pCurrentState->Update(timeDelta);
	}
}

void StateMachine::Late_Update(Float timeDelta)
{
	if (nullptr != m_pCurrentState)
	{
		m_pCurrentState->Late_Update(timeDelta);
	}
}

void StateMachine::Add_State(const wstring& stateTag, const Shared<State>& pState)
{
	if (nullptr == pState || m_States.find(stateTag) != m_States.end())
	{
		return;
	}

	m_States.emplace(stateTag, pState);

	if (nullptr == m_pCurrentState)
	{
		m_pCurrentState = pState;
		m_pCurrentState->Enter();
	}
}

void StateMachine::Change_State(const wstring& stateTag)
{
	auto iter = m_States.find(stateTag);
	if (iter == m_States.end() || m_pCurrentState == iter->second)
	{
		return;
	}

	if (nullptr != m_pCurrentState)
	{
		m_pCurrentState->Exit();
	}

	m_pCurrentState = iter->second;
	m_pCurrentState->Enter();
}

Shared<Component> StateMachine::Clone(void* arg)
{
	return make_shared<StateMachine>(*this);
}

NS_END

RTTR_REGISTRATION
{
	rttr::registration::class_<Engine::StateMachine>("StateMachine")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}
