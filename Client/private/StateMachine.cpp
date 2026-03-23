#include "pch.h"
#include "StateMachine.h"

#include <SpdLogger.h>

StateMachine::StateMachine() : ScriptComponent{} {}
StateMachine::StateMachine(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ScriptComponent{device, context} {}
StateMachine::StateMachine(const StateMachine& rhs)
	: ScriptComponent {rhs} {}

Shared<State> StateMachine::Get_CurrentState()
{
	return m_CurrentState;
}

HRESULT StateMachine::Add_State(const Shared<State>& state)
{
	if (state == nullptr || m_States.contains(state->Get_StateTag()))
	{
		LOG_ERROR(L"Failed to Add State");
		return E_FAIL;
	}

	m_States.emplace(state->Get_StateTag(), state);
	return S_OK;
}

Bool StateMachine::Change_State(const wstring& stateTag)
{
	if (!m_States.contains(stateTag)) 
	{
		LOG_ERROR(L"Failed to Change State");
		return false;
	}

	if (m_States[stateTag]->StateEnterInvoke())
	{
		if (m_CurrentState)
			m_CurrentState->StateExitInvoke();
		m_CurrentState = m_States[stateTag];
		return true;
	}
	return false;
}

HRESULT StateMachine::Initialize_Prototype()
{
	return ScriptComponent::Initialize_Prototype();
}

HRESULT StateMachine::Initialize(void* arg)
{
	return ScriptComponent::Initialize(arg);
}

void StateMachine::On_Destroy()
{
	m_States.clear();
	ScriptComponent::On_Destroy();
}

void StateMachine::Update_State(Float timeDelta)
{
	for (auto& [tag, state] : m_States)
	{
		state->Update(timeDelta);
	}
	for (auto& [tag, state] : m_States)
	{
		state->Late_Update(timeDelta);
	}
}

Shared<StateMachine> StateMachine::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto stateMachine = make_shared<StateMachine>(device, context);
	if (FAILED(stateMachine->Initialize_Prototype()))
	{
		LOG_ERROR("Failed to Create StateMachine");
		return nullptr;
	}
	return stateMachine;
}

Shared<Component> StateMachine::Clone(void* arg)
{
	auto stateMachine = make_shared<StateMachine>();
	if (FAILED(stateMachine->Initialize(arg)))
	{
		LOG_ERROR("Failed to Cloned StateMachine");
		return nullptr;
	}
	return stateMachine;
}
