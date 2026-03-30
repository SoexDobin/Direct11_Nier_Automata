#include "pch.h"
#include "Movement.h"

Movement::Movement() : ScriptComponent{} {}
Movement::Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ScriptComponent{ device, context } {}
Movement::Movement(const Movement& rhs)
	: ScriptComponent{rhs} {}

HRESULT Movement::Initialize_Prototype()
{
	return ScriptComponent::Initialize_Prototype();
}

HRESULT Movement::Initialize(void* arg)
{
	if (nullptr != arg)
	{
		MOVEMENT_DESC& desc = *static_cast<MOVEMENT_DESC*>(arg);
		m_MoveSpeed = desc.moveSpeed;
		m_TurnSpeed = desc.turnSpeed;
		m_TargetDirection = desc.targetDirection;
		m_Velocity = desc.velocity;
		m_Gravity = desc.gravity;
	}

	return ScriptComponent::Initialize(arg);
}

HRESULT Movement::Begin()
{
	return ScriptComponent::Begin();
}

