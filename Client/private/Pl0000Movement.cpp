#include "pch.h"
#include "Pl0000Movement.h"

#include <Game.h>
#include <SpdLogger.h>
#include "Camera.h"
#include "Pl0000Input.h"
#include "Pl0000.h"
#include "Pl0000StateMachine.h"

Pl0000Movement::Pl0000Movement() : Movement{} {}
Pl0000Movement::Pl0000Movement(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Movement{device, context} {}
Pl0000Movement::Pl0000Movement(const Pl0000Movement& rhs) : Movement{rhs} {}

HRESULT Pl0000Movement::Initialize_Prototype()
{
	return Movement::Initialize_Prototype();
}

HRESULT Pl0000Movement::Initialize(void* arg)
{
	if (FAILED(Movement::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Pl0000Movement::Begin()
{
	if (!m_Owner.expired())
	{
		m_OwnerContainer = static_pointer_cast<Pl0000>(m_Owner.lock());
		if (m_OwnerContainer.expired())
		{
			LOG_ERROR(L"Failed To Find Pl0000 Container");
			return E_FAIL;
		}

		m_Input = m_Owner.lock()->Get_Component<Pl0000Input>();
		if (m_Input.expired())
		{
			LOG_ERROR(L"Failed To Find Pl0000Input");
			return E_FAIL;
		}
	}

	return S_OK;
}

void Pl0000Movement::Update_Movement(Float timeDelta)
{
	if (m_Owner.expired() || m_Input.expired()) return;

	Shared<Transform> ownerTransform = m_Owner.lock()->Get_Transform();

	if (!m_IsGrounded)
	{
		m_Velocity.y -= m_Gravity * timeDelta;
	}
	Vector3 physicsDelta = m_Velocity * timeDelta;
	
	TRANSFORM_FRAME transformFrame = m_OwnerContainer.lock()->Get_BodyModelTransform();
	Vector3 rootPositionVelocity = transformFrame.position;

	if (m_IsGrounded)
	{
		m_LastGroundedRootPositionVelocity = rootPositionVelocity;
		m_LastGroundedRootPositionVelocity.y = 0.f; // H O X Y 모르니
	}
	else // grounded
	{
		if (m_CurrentMoveData.isMove)
		{
			rootPositionVelocity = m_LastGroundedRootPositionVelocity;
		}
		else
		{
			rootPositionVelocity = Vector3::Zero;
		}
	}

	if (m_CurrentMoveData.canRotation)
	{
		// 기본적으로는 이동하는 방향(direction)을 회전 타겟으로 잡습니다.
		Vector3 targetRotDir = m_CurrentMoveData.direction;
		// 만약 명시적인 시선 방향(lookDirection) 정보가 들어왔다면 로직을 덮어씌웁니다.
		if (m_CurrentMoveData.lookDirection.LengthSquared() > 0.f)
		{
			targetRotDir = m_CurrentMoveData.lookDirection;
		}
		// 최종적으로 바라볼 방향 벡터가 유효할 때만 회전시킵니다.
		if (targetRotDir.LengthSquared() > 0.f)
		{
			Float targetYaw = atan2f(targetRotDir.x, targetRotDir.z);
			Quaternion targetQuat = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.f, 0.f);
			Float turnSpeed = m_TurnSpeed * timeDelta;
			Quaternion currentQuat = ownerTransform->Get_Quaternion();
			Quaternion nextQuat = Quaternion::Slerp(currentQuat, targetQuat, turnSpeed);
			ownerTransform->Set_Rotation(nextQuat);
		}
	}
	
	Vector3 worldMoveDelta{};
	if (m_CurrentMoveData.useRootMotionDir)
	{
		worldMoveDelta = Vector3::Transform(rootPositionVelocity * -1.f, ownerTransform->Get_Quaternion());
	}
	else
	{
		Float rootSpeed = rootPositionVelocity.Length();
		worldMoveDelta = m_CurrentMoveData.direction * rootSpeed;
	}

	// 💡 루트 모션 스케일 적용 (타격 시 m_RootMotionScale = 0.1f 가 되어 1/10 속도로 느리게 전진)
	worldMoveDelta *= m_RootMotionScale;
	
	Vector3 nextPosition = ownerTransform->Get_Position() + worldMoveDelta * timeDelta + physicsDelta;
	nextPosition += m_CorrectionDelta;
	Reset_Correction();

	Float groundHeight = 0.f; // TODO Nav메시를 통한 y축 판별
	if (nextPosition.y <= groundHeight) {
		nextPosition.y = groundHeight;
		//m_Velocity.y = 0.f;
		m_Velocity = Vector3::Zero;
		m_IsGrounded = true;
	}
	else {
		m_IsGrounded = false;
	}

	ownerTransform->Set_Position(nextPosition);
}

Shared<Pl0000Movement> Pl0000Movement::Create(const ComPtr<ID3D11Device>& device,
                                              const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Pl0000Movement>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000Movement");
		return nullptr;
	}

	return prototype;
}

Shared<Component> Pl0000Movement::Clone(void* arg)
{
	auto instance = make_shared<Pl0000Movement>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000Movement");
		return nullptr;
	}

	return instance;
}


