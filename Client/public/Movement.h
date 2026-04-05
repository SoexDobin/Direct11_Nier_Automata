#pragma once
#include "ScriptComponent.h"

NS_BEGIN(Client)

class CLIENT_DLL Movement abstract : public ScriptComponent
{
	RTTR_ENABLE(ScriptComponent)
public:
	typedef struct tagMovement : public COMPONENT_DESC
	{
		Vector3			velocity{ 0.f, 0.f, 0.f };
		Vector3			targetDirection{ 0.f, 0.f, 0.f };
		Float			moveSpeed{ 0.f };
		Float			turnSpeed{ 0.f };
		Float			gravity{ 30.f };
	} MOVEMENT_DESC;

public:
	explicit Movement();
	explicit Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Movement(const Movement& rhs);
	virtual ~Movement() override = default;

public:
	Bool Is_Grounded() const { return m_IsGrounded; }
	Float Get_GravityScalar() const { return m_Gravity; }
	void Add_Force(const Vector3& impulse) { m_Velocity += impulse; m_IsGrounded = false; }
	void Add_Correction(const Vector3& correction) { m_CorrectionDelta += correction; }
	void Reset_Correction() { m_CorrectionDelta = Vector3::Zero; }

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	HRESULT Begin() override;

public:
	virtual void Update_Movement(Float timeDelta) {};

protected:
	Vector3			m_Velocity{ 0.f, 0.f, 0.f };
	Vector3			m_TargetDirection{ 0.f, 0.f, 0.f };
	Float			m_MoveSpeed{ 0.f };
	Float			m_TurnSpeed{ 0.f };
	Float			m_Gravity{ 30.f };
	Bool			m_IsGrounded{ true };
	Vector3			m_CorrectionDelta{ 0.f };

public:
	Shared<Component> Clone(void* arg = nullptr) PURE;
};

NS_END